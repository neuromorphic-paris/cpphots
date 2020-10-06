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


std::tuple<double, double, double> test_training(const std::string& folder, bool sequential, bool multi, const cpphots::LayerInitializer& initializer) {

    cpphots::Network network(128, 128, 2,
                        2,
                        2, 2, 2,
                        1000, 5,
                        16, 2);

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

    network.toggleLearningAll(false);

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
        int n_trainings = 100;

        std::ofstream unif_sequential_file("unif_sequential.csv");
        unif_sequential_file << "acc1,acc2,acc3" << std::endl;
        for (int i = 0; i < n_trainings; i++) {
            std::cout << "training (seq) " << i+1 << "/" << n_trainings << std::endl;
            auto res = test_training(datafolder, true, false, cpphots::LayerUniformInitializer{});
            unif_sequential_file << std::get<0>(res) << "," << std::get<1>(res) << "," << std::get<2>(res) << std::endl;
        }
        unif_sequential_file.close();

        std::ofstream unif_sequentialmulti_file("unif_sequential_multi.csv");
        unif_sequentialmulti_file << "acc1,acc2,acc3" << std::endl;
        for (int i = 0; i < n_trainings; i++) {
            std::cout << "training (seq-multi) " << i+1 << "/" << n_trainings << std::endl;
            auto res = test_training(datafolder, true, true, cpphots::LayerUniformInitializer{});
            unif_sequentialmulti_file << std::get<0>(res) << "," << std::get<1>(res) << "," << std::get<2>(res) << std::endl;
        }
        unif_sequentialmulti_file.close();

        std::ofstream unif_oneshot_file("unif_oneshot.csv");
        unif_oneshot_file << "acc1,acc2,acc3" << std::endl;
        for (int i = 0; i < n_trainings; i++) {
            std::cout << "training (one) " << i+1 << "/" << n_trainings << std::endl;
            auto res = test_training(datafolder, false, false, cpphots::LayerUniformInitializer{});
            unif_oneshot_file << std::get<0>(res) << "," << std::get<1>(res) << "," << std::get<2>(res) << std::endl;
        }
        unif_oneshot_file.close();

        std::ofstream unif_oneshotmulti_file("unif_oneshot_multi.csv");
        unif_oneshotmulti_file << "acc1,acc2,acc3" << std::endl;
        for (int i = 0; i < n_trainings; i++) {
            std::cout << "training (one-multi) " << i+1 << "/" << n_trainings << std::endl;
            auto res = test_training(datafolder, false, true, cpphots::LayerUniformInitializer{});
            unif_oneshotmulti_file << std::get<0>(res) << "," << std::get<1>(res) << "," << std::get<2>(res) << std::endl;
        }
        unif_oneshotmulti_file.close();


        std::ofstream pp_sequential_file("pp_sequential.csv");
        pp_sequential_file << "acc1,acc2,acc3" << std::endl;
        for (int i = 0; i < n_trainings; i++) {
            std::cout << "training (seq) " << i+1 << "/" << n_trainings << std::endl;
            auto res = test_training(datafolder, true, false, cpphots::LayerPlusPlusInitializer{});
            pp_sequential_file << std::get<0>(res) << "," << std::get<1>(res) << "," << std::get<2>(res) << std::endl;
        }
        pp_sequential_file.close();

        std::ofstream pp_sequentialmulti_file("pp_sequential_multi.csv");
        pp_sequentialmulti_file << "acc1,acc2,acc3" << std::endl;
        for (int i = 0; i < n_trainings; i++) {
            std::cout << "training (seq-multi) " << i+1 << "/" << n_trainings << std::endl;
            auto res = test_training(datafolder, true, true, cpphots::LayerPlusPlusInitializer{});
            pp_sequentialmulti_file << std::get<0>(res) << "," << std::get<1>(res) << "," << std::get<2>(res) << std::endl;
        }
        pp_sequentialmulti_file.close();

        std::ofstream pp_oneshot_file("pp_oneshot.csv");
        pp_oneshot_file << "acc1,acc2,acc3" << std::endl;
        for (int i = 0; i < n_trainings; i++) {
            std::cout << "training (one) " << i+1 << "/" << n_trainings << std::endl;
            auto res = test_training(datafolder, false, false, cpphots::LayerPlusPlusInitializer{});
            pp_oneshot_file << std::get<0>(res) << "," << std::get<1>(res) << "," << std::get<2>(res) << std::endl;
        }
        pp_oneshot_file.close();

        std::ofstream pp_oneshotmulti_file("pp_oneshot_multi.csv");
        pp_oneshotmulti_file << "acc1,acc2,acc3" << std::endl;
        for (int i = 0; i < n_trainings; i++) {
            std::cout << "training (one-multi) " << i+1 << "/" << n_trainings << std::endl;
            auto res = test_training(datafolder, false, true, cpphots::LayerPlusPlusInitializer{});
            pp_oneshotmulti_file << std::get<0>(res) << "," << std::get<1>(res) << "," << std::get<2>(res) << std::endl;
        }
        pp_oneshotmulti_file.close();

    } else {

        auto res = test_training(datafolder, true, true, cpphots::LayerPlusPlusInitializer{});
        std::cout << "acc1 = " << std::get<0>(res) << ", acc2 = " << std::get<1>(res) << ", acc3 = " << std::get<2>(res) << std::endl;

    }

    return 0;

}