#ifndef TESTING_H
#define TESTING_H

#include <vector>
#include <string>
#include <utility>

#include "layer.h"
#include "network.h"
#include "classification.h"


namespace cpphots {


template<typename P>
Events process(P& processor, const Events& events) {

    processor.reset();

    Events ret;
    for (const auto& ev : events) {
        Events nev = processor.process(ev);
        ret.insert(ret.end(), nev.begin(), nev.end());
    }

    return ret;

}

template<typename P>
std::vector<Events> process(P& processor, const std::vector<Events>& events) {

    std::vector<Events> ret;

    for (const auto& evts : events) {
        ret.push_back(process(processor, evts));
    }

    return ret;

}

// training
void train_oneshot(Network& network, const std::vector<Events>& training_events, const ClustererInitializerType& initializer, bool use_all = true);
void train_sequential(Network& network, const std::vector<Events>& training_events, const ClustererInitializerType& initializer, bool use_all = true);

}

#endif