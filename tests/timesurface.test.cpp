#include "../src/time_surface.h"
#include "../src/events_utils.h"

#include <iostream>


int main() {

    // load data
    cpphots::Events events = cpphots::loadFromFile("trcl0.es");

    // create time surface
    cpphots::TimeSurface ts(32, 32, 2, 2, 1000);

    float normsum = 0.;
    float goodsum = 0.;

    unsigned int processed = 0;
    unsigned int goodevents = 0;
    for (auto& ev : events) {
        if (ev.p == 0) {
            continue;
        }
        auto nts = ts.updateAndCompute(ev.x, ev.y, ev.t);
        float norm = nts.first.matrix().norm();
        normsum += norm;
        if (nts.second) {
            goodsum += norm;
            goodevents++;
        }
        processed++;
    }
    std::cout << "      sum: " << normsum << std::endl;
    std::cout << "  goodsum: " << goodsum << std::endl;
    std::cout << "processed: " << processed << std::endl;
    std::cout << "     good: " << goodevents << std::endl;

    if (std::abs(normsum - 4740.313427652784) >= 0.1)
        return 1;

    if (std::abs(goodsum - 4562.696117657931) >= 0.1)
        return 1;
    
    if (goodevents != 1783)
        return 1;

    return 0;
    
}