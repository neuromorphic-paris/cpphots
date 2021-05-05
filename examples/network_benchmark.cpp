/**
 * @file network_benchmark.cpp
 * @brief Computes the overhead of using the Layer and Network classes
 */
#include <chrono>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <cpphots/layer.h>
#include <cpphots/network.h>

#include "commons.h"


using Results = std::vector<std::vector<double>>;

void save_results_csv(const std::string& filename, const std::vector<unsigned int>& layers, const std::vector<double> events, const Results& results) {

    std::ofstream csv(filename);

    auto width = csv.width();
    auto prec = csv.precision();

    csv << "layers";
    for (double ne : events) {
        csv << ",ne" << std::scientific << std::setw(5) << std::setprecision(0) << ne;
    }
    csv << std::endl;
    csv << std::defaultfloat << std::setw(width) << std::setprecision(prec);

    for (size_t l = 0; l < layers.size(); l++) {
        csv << l;
        for (double r : results[l]) {
            csv << "," << r;
        }
        csv << std::endl;
    }

}

double test_components(unsigned int num_layers, unsigned int num_events) {

    std::vector<cpphots::TimeSurfacePool> pools;
    std::vector<cpphots::HOTSClusterer> clusts;

    auto initializer = cpphots::ClustererRandomInitializer(5, 5);

    auto evgen = getRandomEventGenerator(50, 50);

    for (unsigned int l = 0; l < num_layers; l++) {
        pools.push_back(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 50, 50, 2, 2, 100));
        clusts.push_back(cpphots::HOTSClusterer(2));
        initializer(clusts.back(), {});
    }

    auto start = std::chrono::system_clock::now();
    for (unsigned int i = 0; i < num_events; i++) {
        auto ev = evgen();

        for (unsigned int l = 0; l < num_layers; l++) {
            auto [ts, good] = pools[l].updateAndCompute(ev);
            uint16_t k = clusts[l].cluster(ts);
            ev.p = k;
        }

    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    return diff.count();

}

double test_layer(unsigned int num_layers, unsigned int num_events) {

    std::vector<cpphots::LayerPtr> layers;

    auto initializer = cpphots::ClustererRandomInitializer(5, 5);

    auto evgen = getRandomEventGenerator(50, 50);

    for (unsigned int l = 0; l < num_layers; l++) {
        auto layer = cpphots::create_layer_ptr(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 50, 50, 2, 2, 100),
                                               cpphots::HOTSClusterer(2));
        initializer(*dynamic_cast<cpphots::ClustererBase*>(layer.get()), {});
        layers.push_back(layer);
    }

    auto start = std::chrono::system_clock::now();
    for (unsigned int i = 0; i < num_events; i++) {
        auto ev = evgen();

        for (unsigned int l = 0; l < num_layers; l++) {
            auto evts = layers[l]->process(ev, true);
            ev = evts[0];
        }

    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    return diff.count();

}

double test_network(unsigned int num_layers, unsigned int num_events) {

    cpphots::Network network;

    auto initializer = cpphots::ClustererRandomInitializer(5, 5);

    auto evgen = getRandomEventGenerator(50, 50);

    for (unsigned int l = 0; l < num_layers; l++) {
        auto layer = cpphots::create_layer_ptr(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 50, 50, 2, 2, 100),
                                               cpphots::HOTSClusterer(2));
        initializer(*dynamic_cast<cpphots::ClustererBase*>(layer.get()), {});
        network.addLayer(layer);
    }

    auto start = std::chrono::system_clock::now();
    for (unsigned int i = 0; i < num_events; i++) {
        auto ev = evgen();
        network.process(ev, true);
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    return diff.count();

}


Results print_table(const std::string& name, const std::vector<unsigned int>& layers, const std::vector<double> events, bool average = false) {

    Results res;
    res.resize(layers.size(), std::vector<double>(events.size()));

    std::cout << std::endl;
    std::cout.width(10);
    std::cout << name << " |";
    for (double ne : events) {
        std::cout << std::scientific << std::setw(9) << std::setprecision(0) << std::setfill(' ') << ne << " |";
    }
    std::cout << std::endl;

    std::cout << "-----------+";
    for (double ne : events) {
        std::cout << "----------+";
    }
    std::cout << std::endl;

    for (size_t l = 0; l < layers.size(); l++) {
        unsigned int nl = layers[l];
        std::cout.width(10);
        std::cout << (std::to_string(nl) + " layers") << " |";
        for (size_t e = 0; e < events.size(); e++) {
            double ne = events[e];

            // run the right test
            double tt = 0.0;
            if (name == "COMPONENTS") {
                tt = test_components(nl, ne);
            } else if (name == "LAYER") {
                tt = test_layer(nl, ne);
            } else if (name == "NETWORK") {
                tt = test_network(nl, ne);
            }

            // adjust for average if requested
            if (average) {
                tt = tt * 1e6 / nl / ne;
            }

            res[l][e] = tt;
            std::cout << std::fixed << std::setw(9) << std::setprecision(3) << std::setfill(' ') << tt << " |";
            std::cout.flush();
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    return res;

}


int main(int argc, char* argv[]) {

    if (argc > 3) {
        std::cerr << "Too many arguments. Only arguments available are '--save' and '--avg'" << std::endl;
        return 1;
    }

    std::string arg1, arg2;
    if (argc > 1) {
        arg1 = std::string(argv[1]);
    }
    if (argc > 2) {
        arg2 = std::string(argv[2]);
    }

    bool save = false;
    if (arg1 == "--save" || arg2 == "--save") {
        save = true;
    }

    bool avg = false;
    if (arg1 == "--avg" || arg2 == "--avg") {
        avg = true;
    }

    std::vector<unsigned int> layers{1, 2, 5, 10};
    std::vector<double> events{1e6, 10e6, 100e6, 1e9};

    for (auto name : {"COMPONENTS", "LAYER", "NETWORK"}) {
        auto res = print_table(name, layers, events, avg);
        if (save) {
            if (avg) {
                save_results_csv(std::string(name) + "_avg.csv", layers, events, res);
            } else {
                save_results_csv(std::string(name) + ".csv", layers, events, res);
            }
        }
    }

    return 0;

}