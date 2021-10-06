/**
 * @file clustering_benchmark.cpp
 * @brief Performance comparison between different clustering algorithms
 */
#include <iostream>
#include <chrono>
#include <iomanip>

#include <cpphots/time_surface.h>
#include <cpphots/layer.h>
#include <cpphots/clustering/cosine.h>
#include <cpphots/clustering/kmeans.h>
#ifdef CPPHOTS_WITH_PEREGRINE
#include <cpphots/clustering/gmm.h>
#endif
#include <cpphots/run.h>

#include "commons.h"


std::pair<double, double> measure_times(cpphots::Layer& layer, size_t n_training, size_t n_events) {

    auto event_gen = getRandomEventGenerator(100, 100, 0);

    // train
    cpphots::Events training_evs;
    for (size_t i = 0; i < n_training; i++) {
        training_evs.push_back(event_gen());
    }

    auto start = std::chrono::system_clock::now();
    cpphots::layerSeedCentroids(cpphots::ClustererAFKMC2Seeding(5), layer, training_evs, false);
    layer.toggleLearning(true);
    for (const auto& ev : training_evs) {
        layer.process(ev, true);
    }
    layer.toggleLearning(false);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> time_training = end - start;

    // process
    start = std::chrono::system_clock::now();
    for (size_t i = 0; i < n_events; i++) {
        layer.process(event_gen(), true);
    }
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> time_processing = end - start;

    return {time_training.count(), time_processing.count()};

}

int main() {

    size_t n_training = 10000;
    size_t n_events = 10e6;

    std::cout << "       |  training | execution" << std::endl;

    {
        cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(100, 100, 5, 5, 500., 1),
                             new cpphots::CosineClusterer(10));
        auto [tr, ex] = measure_times(layer, n_training, n_events);
        std::cout << " cosine | " << std::setw(9) << std::setprecision(5) << tr << " | " << std::setw(9) << std::setprecision(5) << ex << std::endl;
    }

    #ifdef CPPHOTS_WITH_PEREGRINE

    {
        cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(100, 100, 5, 5, 500., 1),
                             new cpphots::GMMClusterer(cpphots::GMMClusterer::S_GMM, 10, 5, 8, 0.01, 20));
        auto [tr, ex] = measure_times(layer, n_training, n_events);
        std::cout << "  S-GMM | " << std::setw(9) << std::setprecision(5) << tr << " | " << std::setw(9) << std::setprecision(5) << ex << std::endl;
    }

    {
        cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(100, 100, 5, 5, 500., 1),
                             new cpphots::GMMClusterer(cpphots::GMMClusterer::U_S_GMM, 10, 5, 8, 0.01, 20));
        auto [tr, ex] = measure_times(layer, n_training, n_events);
        std::cout << " uS-GMM | " << std::setw(9) << std::setprecision(5) << tr << " | " << std::setw(9) << std::setprecision(5) << ex << std::endl;
    }

    #endif

    {
        cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(100, 100, 5, 5, 500., 1),
                             new cpphots::KMeansClusterer(10, 20));
        auto [tr, ex] = measure_times(layer, n_training, n_events);
        std::cout << "k-means | " << std::setw(9) << std::setprecision(5) << tr << " | " << std::setw(9) << std::setprecision(5) << ex << std::endl;
    }

    return 0;

}