/**
 * @file poker_dvs.cpp
 * @brief Classification on POKER-DVS
 * 
 * Example of how to use cpphots for a classification task, using the dataset from http://www2.imse-cnm.csic.es/caviar/POKERDVS.html
 * A version of the dataset in EventStream format can be downloaded from https://www.dropbox.com/s/6700gh70mbwzxa0/poker-dvs-eventstream.zip?dl=0
 */

#include <iostream>
#include <fstream>

#include <cpphots/network.h>
#include <cpphots/events_utils.h>
#include <cpphots/classification.h>
#include <cpphots/run.h>


cpphots::Features process_file(cpphots::Network& network, const std::string& filename) {

    // load file
    auto events = cpphots::loadFromFile(filename);

    // run network
    network.reset();
    for (const auto& ev : events) {

        // there are some events outside the range
        if (ev.x >= 32 || ev.y >= 32) {
            continue;
        }

        network.process(ev);
    }

    auto feats = network.back<cpphots::ClustererBase>().getHistogram();

    return feats;

}

// test suite
// takes a trained network, a trained classifier and a test set (vector<filename, label>)
double compute_accuracy(cpphots::Network& network, const cpphots::Classifier& classifier, const std::vector<std::pair<std::string, std::string>>& test_set) {

    double acc = 0;

    for (auto sample : test_set) {

        // classify
        auto predicted = classifier.classifyName(process_file(network, sample.first));

        if (predicted == sample.second) {
            acc++;
        }

    }

    return acc / test_set.size();

}

std::vector<double> compute_accuracy(cpphots::Network& network, const std::vector<cpphots::Classifier*>& classifiers, const std::vector<std::pair<std::string, std::string>>& test_set) {

    std::vector<double> acc(classifiers.size());

    for (auto sample : test_set) {

        auto feats = process_file(network, sample.first);

        // classify
        for (size_t i = 0; i < classifiers.size(); i ++) {
             auto predicted = classifiers[i]->classifyName(feats);

            if (predicted == sample.second) {
                acc[i]++;
            }

        }

    }

    for (size_t i = 0; i < classifiers.size(); i ++) {
        acc[i] = acc[i] / test_set.size();
    }


    return acc;

}

// shorthand for training
void train(cpphots::Network& network, const std::vector<std::string>& training_set, const cpphots::ClustererInitializerType& initializer, bool use_all = true) {

    // load all traning set
    std::vector<cpphots::Events> training_events;
    for (auto& filename : training_set) {
        training_events.push_back(cpphots::loadFromFile(filename));
    }

    cpphots::train(network, training_events, initializer, use_all);

}


std::vector<std::pair<std::string, std::string>> poker_dvs_trainset(const std::string& folder) {

    std::vector<std::pair<std::string, std::string>> ret;

    std::vector<std::string> pips{"cl", "di", "he", "sp"};

    std::unordered_map<std::string, std::string> longer{{"cl", "club"},
                                                        {"di", "diamond"},
                                                        {"he", "heart"},
                                                        {"sp", "spade"}};

    for (auto p : pips) {
        ret.push_back({folder + "/tr" + p + "0.es", longer[p]});
    }

    return ret;

}


std::vector<std::pair<std::string, std::string>> poker_dvs_testset(const std::string& folder) {

    std::vector<std::pair<std::string, std::string>> ret;

    std::vector<std::string> pips{"cl", "di", "he", "sp"};

    std::unordered_map<std::string, std::string> longer{{"cl", "club"},
                                                        {"di", "diamond"},
                                                        {"he", "heart"},
                                                        {"sp", "spade"}};

    for (auto p : pips) {
        for (int i = 0; i < 5; i++) {
            ret.push_back({folder + "/te" + p + std::to_string(i) + ".es", longer[p]});
        }
    }

    for (auto p : pips) {
        for (int i = 1; i < 12; i++) {
            ret.push_back({folder + "/tr" + p + std::to_string(i) + ".es", longer[p]});
        }
    }

    return ret;

}


std::vector<std::pair<std::string, std::string>> poker_dvs_all(const std::string& folder) {

    auto tr = poker_dvs_trainset(folder);
    auto te = poker_dvs_testset(folder);

    tr.insert(tr.end(), te.begin(), te.end());

    return tr;

}


std::tuple<double, double, double> test_training(const std::string& folder, bool multi, const cpphots::ClustererInitializerType& initializer) {

    cpphots::Network network;
    network.addLayer(cpphots::LinearTimeSurfacePool(2, 32, 32, 2, 2, 1000),
                     cpphots::HOTSClusterer(16));
    network.addLayer(cpphots::LinearTimeSurfacePool(16, 32, 32, 4, 4, 5000),
                     cpphots::HOTSClusterer(32));

    auto train_set = poker_dvs_trainset(folder);

    // train network
    train(network,
          {train_set[0].first,
           train_set[1].first,
           train_set[2].first,
           train_set[3].first},
          initializer,
          multi);

    for (auto cl : network.view<cpphots::ClustererBase>()) {
        cl->toggleLearning(false);
    }

    // prepare classifiers
    cpphots::StandardClassifier classifier1({"club", "diamond", "heart", "spade"});

    // train classifier
    classifier1.setClassFeatures(train_set[0].second, process_file(network, train_set[0].first));
    classifier1.setClassFeatures(train_set[1].second, process_file(network, train_set[1].first));
    classifier1.setClassFeatures(train_set[2].second, process_file(network, train_set[2].first));
    classifier1.setClassFeatures(train_set[3].second, process_file(network, train_set[3].first));

    cpphots::NormalizedClassifier classifier2(&classifier1);
    cpphots::BhattacharyyaClassifier classifier3(&classifier1);

    // run test suite
    auto test_set = poker_dvs_testset(folder);
    auto acc = compute_accuracy(network, {&classifier1, &classifier2, &classifier3}, test_set);

    return std::make_tuple(acc[0], acc[1], acc[2]);

}


int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Please specify the folder with data" <<  std::endl;
        return 1;
    }

    std::string datafolder(argv[1]);

    if (argc > 2 && std::string(argv[2]) == "--stats") {

        // run training some times
        int n_trainings;
        if (argc > 3) {
            n_trainings = std::atoi(argv[3]);
        } else {
            n_trainings = 100;
        }

        const std::vector<std::tuple<std::string, std::string, bool, cpphots::ClustererInitializerType>> test_cases{
        
            {"afkmc2_sequential.csv", "seq", false, cpphots::ClustererAFKMC2Initializer(5)},
            {"afkmc2_sequential_multi.csv", "seq-multi", true, cpphots::ClustererAFKMC2Initializer(5)},

            {"unif_sequential.csv", "seq", false, cpphots::ClustererUniformInitializer},
            {"unif_sequential_multi.csv", "seq-multi", true, cpphots::ClustererUniformInitializer},
        
            {"pp_sequential.csv", "seq", false, cpphots::ClustererPlusPlusInitializer},
            {"pp_sequential_multi.csv", "seq-multi", true, cpphots::ClustererPlusPlusInitializer},
        
        };
 
        for (auto& tcase : test_cases) {

            std::ofstream file(std::get<0>(tcase));
            file << "acc1,acc2,acc3" << std::endl;
            for (int i = 0; i < n_trainings; i++) {
                std::cout << "training (" << std::get<1>(tcase) << ") " << i+1 << "/" << n_trainings << std::endl;
                auto res = test_training(datafolder, std::get<2>(tcase), std::get<3>(tcase));
                file << std::get<0>(res) << "," << std::get<1>(res) << "," << std::get<2>(res) << std::endl;
            }
            file.close();
        }

    } else {

        auto res = test_training(datafolder, true, cpphots::ClustererPlusPlusInitializer);
        std::cout << "acc1 = " << std::get<0>(res) << ", acc2 = " << std::get<1>(res) << ", acc3 = " << std::get<2>(res) << std::endl;

    }

    return 0;

}