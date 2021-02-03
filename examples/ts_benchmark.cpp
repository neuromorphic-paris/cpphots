#include <iostream>
#include <chrono>

#include <cpphots/events_utils.h>
#include <cpphots/time_surface.h>
#include <cpphots/layer.h>
#include <cpphots/network.h>

#include "commons.h"


void perform_test_ts(uint16_t sz, uint16_t r, cpphots::TimeSurfaceScalarType tau, unsigned int repetitions = 5) {

    double time = 0.0;

    for (unsigned int i = 0; i < repetitions; i++) {

        auto event_gen = getRandomEventGenerator(sz, sz);

        cpphots::LinearTimeSurface ts(sz, sz, r, r, tau);

        auto start = std::chrono::system_clock::now();
        for (size_t i = 0; i < 1e6; i++) {
            ts.updateAndCompute(event_gen());
        }
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end - start;

        time += diff.count();

    }

    time /= repetitions;

    std::cout << "," << time;

}

void perform_test_p(uint16_t sz, uint16_t r, cpphots::TimeSurfaceScalarType tau, unsigned int repetitions = 5) {

    double time = 0.0;

    for (unsigned int i = 0; i < repetitions; i++) {

        auto event_gen = getRandomEventGenerator(sz, sz);

        auto tsp = cpphots::create_pool<cpphots::LinearTimeSurface>(1, sz, sz, r, r, tau);

        auto start = std::chrono::system_clock::now();
        for (size_t i = 0; i < 1e6; i++) {
            tsp.updateAndCompute(event_gen());
        }
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end - start;

        time += diff.count();

    }

    time /= repetitions;

    std::cout << "," << time;

}

void perform_test_l(uint16_t sz, uint16_t r, cpphots::TimeSurfaceScalarType tau, unsigned int repetitions = 5) {

    double time = 0.0;

    for (unsigned int i = 0; i < repetitions; i++) {

        auto event_gen = getRandomEventGenerator(sz, sz);

        auto layer = cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(1, sz, sz, r, r, tau));

        auto start = std::chrono::system_clock::now();
        for (size_t i = 0; i < 1e6; i++) {
            layer.process(event_gen());
        }
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end - start;

        time += diff.count();

    }

    time /= repetitions;

    std::cout << "," << time;

}

void perform_test_n(uint16_t sz, uint16_t r, cpphots::TimeSurfaceScalarType tau, unsigned int repetitions = 5) {

    double time = 0.0;

    for (unsigned int i = 0; i < repetitions; i++) {

        auto event_gen = getRandomEventGenerator(sz, sz);

        cpphots::Network net;
        net.addLayer(cpphots::create_pool<cpphots::LinearTimeSurface>(1, sz, sz, r, r, tau));

        auto start = std::chrono::system_clock::now();
        for (size_t i = 0; i < 1e6; i++) {
            net.process(event_gen());
        }
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end - start;

        time += diff.count();

    }

    time /= repetitions;

    std::cout << "," << time;

}

int main() {

    std::cout << "sz,r,tau,ts,p,l,n" << std::endl;

    for (auto sz : {32, 64, 346}) {
        for (auto r : {2, 4, 8, 16}) {
            for (auto tau : {50., 100., 200., 500.}) {
                std::cout << sz << "," << r << "," << tau;
                perform_test_ts(sz, r, tau);
                perform_test_p(sz, r, tau);
                perform_test_l(sz, r, tau);
                perform_test_n(sz, r, tau);
                std::cout << std::endl;
            }
        }
    }

    return 0;

}