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


std::tuple<double, double, double> test_training(const std::string& folder, bool sequential, bool multi, const cpphots::ClustererInitializerType& initializer) {

    cpphots::Network network;
    auto l1 = cpphots::create_layer(cpphots::TimeSurfacePool(32, 32, 2, 2, 1000, 2),
                                    cpphots::HOTSClusterer(16));
    auto l2 = cpphots::create_layer(cpphots::TimeSurfacePool(32, 32, 4, 4, 5000, 16),
                                    cpphots::HOTSClusterer(32));
    network.addLayer(l1);
    network.addLayer(l2);

    auto train_set = poker_dvs_trainset(folder);

    // train network
    if (sequential) {
        cpphots::train_sequential(network,
                                  {train_set[0].first,
                                   train_set[1].first,
                                   train_set[2].first,
                                   train_set[3].first},
                                  initializer,
                                  multi);
    } else {
        cpphots::train_oneshot(network,
                               {train_set[0].first,
                                train_set[1].first,
                                train_set[2].first,
                                train_set[3].first},
                               initializer,
                               multi);
    }

    for (auto cl : network.view<cpphots::HOTSClusterer>()) {
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

        const std::vector<std::tuple<std::string, std::string, bool, bool, cpphots::ClustererInitializerType>> test_cases{
        
            {"afkmc2_sequential.csv", "seq", true, false, cpphots::ClustererAFKMC2Initializer(5)},
            {"afkmc2_sequential_multi.csv", "seq-multi", true, true, cpphots::ClustererAFKMC2Initializer(5)},
            {"afkmc2_oneshot.csv", "one", false, false, cpphots::ClustererAFKMC2Initializer(5)},
            {"afkmc2_oneshot_multi.csv", "one-multi", false, true, cpphots::ClustererAFKMC2Initializer(5)},

            {"unif_sequential.csv", "seq", true, false, cpphots::ClustererUniformInitializer},
            {"unif_sequential_multi.csv", "seq-multi", true, true, cpphots::ClustererUniformInitializer},
            {"unif_oneshot.csv", "one", false, false, cpphots::ClustererUniformInitializer},
            {"unif_oneshot_multi.csv", "one-multi", false, true, cpphots::ClustererUniformInitializer},
        
            {"pp_sequential.csv", "seq", true, false, cpphots::ClustererPlusPlusInitializer},
            {"pp_sequential_multi.csv", "seq-multi", true, true, cpphots::ClustererPlusPlusInitializer},
            {"pp_oneshot.csv", "one", false, false, cpphots::ClustererPlusPlusInitializer},
            {"pp_oneshot_multi.csv", "one-multi", false, true, cpphots::ClustererPlusPlusInitializer}
        
        };
 
        for (auto& tcase : test_cases) {

            std::ofstream file(std::get<0>(tcase));
            file << "acc1,acc2,acc3" << std::endl;
            for (int i = 0; i < n_trainings; i++) {
                std::cout << "training (" << std::get<1>(tcase) << ") " << i+1 << "/" << n_trainings << std::endl;
                auto res = test_training(datafolder, std::get<2>(tcase), std::get<3>(tcase), std::get<4>(tcase));
                file << std::get<0>(res) << "," << std::get<1>(res) << "," << std::get<2>(res) << std::endl;
            }
            file.close();
        }

    } else {

        auto res = test_training(datafolder, false, true, cpphots::ClustererPlusPlusInitializer);
        std::cout << "acc1 = " << std::get<0>(res) << ", acc2 = " << std::get<1>(res) << ", acc3 = " << std::get<2>(res) << std::endl;

    }

    return 0;

}