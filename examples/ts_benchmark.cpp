#include <random>
#include <functional>
#include <iostream>
#include <chrono>

#include <cpphots/time_surface.h>
#include <cpphots/events_utils.h>


std::function<cpphots::event()> getRandomEventGenerator(uint16_t w, uint16_t h, uint16_t seed = 0) {

    std::mt19937 gen(seed);

    uint64_t lastt = 0;

    std::uniform_int_distribution<uint16_t> distx(0, w);
    std::uniform_int_distribution<uint16_t> disty(0, h);
    std::uniform_int_distribution<uint64_t> distt(1, 10);

    return [gen, lastt, distx, disty, distt] () mutable {
        cpphots::event ev;
        lastt += distt(gen);
        ev.t = lastt;
        ev.x = distx(gen);
        ev.y = disty(gen);
        ev.p = 0;
        return ev;
    };

}

void perform_test(uint16_t sz, uint16_t r, cpphots::TimeSurfaceScalarType tau, unsigned int repetitions = 5) {

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

    std::cout << sz << "," << r << "," << tau << "," << time << std::endl;

}

int main() {

    std::cout << "sz,r,tau,t" << std::endl;

    for (auto sz : {32, 64, 346}) {
        for (auto r : {2, 4, 8, 16}) {
            for (auto tau : {50., 100., 200., 500.}) {
                perform_test(sz, r, tau);
            }
        }
    }

    return 0;

}