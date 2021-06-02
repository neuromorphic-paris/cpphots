/**
 * @file clustering_benchmark.cpp
 * @brief Performance comparison between different clustering algorithms
 */
#include <iostream>
#include <chrono>
#include <iomanip>

#include <cpphots/time_surface.h>
#include <cpphots/layer.h>
#include <cpphots/clustering.h>
#include <cpphots/gmm_clustering.h>
#include <cpphots/run.h>

#include "commons.h"

template <typename C>
std::pair<double, double> measure_times(cpphots::Layer<cpphots::TimeSurfacePool, C>& layer, size_t n_training, size_t n_events) {

    auto event_gen = getRandomEventGenerator(100, 100, 0);

    // train
    cpphots::Events training_evs;
    for (size_t i = 0; i < n_training; i++) {
        training_evs.push_back(event_gen());
    }

    auto start = std::chrono::system_clock::now();
    cpphots::layerInitializePrototypes(cpphots::ClustererAFKMC2Initializer(5), layer, layer, training_evs, false);
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
        auto layer = cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(100, 100, 5, 5, 500., 1),
                                           cpphots::HOTSClusterer(10));
        auto [tr, ex] = measure_times(layer, n_training, n_events);
        std::cout << "  HOTS | " << std::setw(9) << std::setprecision(5) << tr << " | " << std::setw(9) << std::setprecision(5) << ex << std::endl;
    }

    {
        auto layer = cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(100, 100, 5, 5, 500., 1),
                                           cpphots::GMMClusterer(cpphots::GMMClusterer::S_GMM, 10, 5, 8, 20));
        auto [tr, ex] = measure_times(layer, n_training, n_events);
        std::cout << " S-GMM | " << std::setw(9) << std::setprecision(5) << tr << " | " << std::setw(9) << std::setprecision(5) << ex << std::endl;
    }

    {
        auto layer = cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(100, 100, 5, 5, 500., 1),
                                           cpphots::GMMClusterer(cpphots::GMMClusterer::U_S_GMM, 10, 5, 8, 20));
        auto [tr, ex] = measure_times(layer, n_training, n_events);
        std::cout << "uS-GMM | " << std::setw(9) << std::setprecision(5) << tr << " | " << std::setw(9) << std::setprecision(5) << ex << std::endl;
    }

    return 0;

}