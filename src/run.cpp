#include "cpphots/run.h"

#include <iostream>

#include "cpphots/events_utils.h"


namespace cpphots {

void train_oneshot(Network& network, const std::vector<Events>& training_events, const ClustererInitializerType& initializer, bool use_all) {

    auto network_clusterers = network.viewFull<ClustererBase>();
    auto network_tspools = network.viewFull<TimeSurfacePool>();

    // prototypes initialization
    std::vector<Events> init_events = training_events;
    for (size_t l = 0; l < network.getNumLayers(); l++) {

        if (network_clusterers[l]) {
            // learn prototypes for this layer
            if (use_all)
                layerInitializePrototypes(initializer, *network_tspools[l], *network_clusterers[l], init_events);
            else
                layerInitializePrototypes(initializer, *network_tspools[l], *network_clusterers[l], init_events[0]);
        }

        // generate events for the next layer
        init_events = process(network.getLayer(l), init_events);

    }

    // learning
    for (auto cl : network.view<ClustererBase>()) {
        cl->toggleLearning(true);
    }
    cpphots::process(network, training_events);
    for (auto cl : network.view<ClustererBase>()) {
        cl->toggleLearning(false);
    }

}

void train_sequential(Network& network, const std::vector<Events>& training_events, const ClustererInitializerType& initializer, bool use_all) {

    auto network_clusterers = network.viewFull<ClustererBase>();
    auto network_tspools = network.viewFull<TimeSurfacePool>();

    std::vector<Events> _training_events = training_events;

    for (size_t l = 0; l < network.getNumLayers(); l++) {

        if (network_clusterers[l]) {
            // learn prototypes for this layer
            if (use_all)
                layerInitializePrototypes(initializer, *network_tspools[l], *network_clusterers[l], _training_events);
            else
                layerInitializePrototypes(initializer, *network_tspools[l], *network_clusterers[l], _training_events[0]);

            // train
            network_clusterers[l]->toggleLearning(true);
            process(network.getLayer(l), _training_events);
            network_clusterers[l]->toggleLearning(false);

        }
        // genereate events for the next layer
        _training_events = process(network.getLayer(l), _training_events);

    }

}

}
