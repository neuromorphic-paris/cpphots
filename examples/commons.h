#ifndef CPPHOTS_EXAMPLES_COMMONS_H
#define CPPHOTS_EXAMPLES_COMMONS_H

#include <random>
#include <functional>

#include <cpphots/events_utils.h>


std::function<cpphots::event()> getRandomEventGenerator(uint16_t w, uint16_t h, uint16_t seed = std::random_device{}()) {

    std::mt19937 gen(seed);

    uint64_t lastt = 0;

    std::uniform_int_distribution<uint16_t> distx(0, w-1);
    std::uniform_int_distribution<uint16_t> disty(0, h-1);
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

#endif