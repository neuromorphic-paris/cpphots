#include "cpphots/run.h"

#include "cpphots/events_utils.h"


namespace cpphots {

void train(Network& network, Events training_events, const ClustererInitializerType& initializer) {

    auto network_clusterers = network.viewFull<ClustererBase>();
    auto network_tspools = network.viewFull<TimeSurfacePoolBase>();

    for (size_t l = 0; l < network.getNumLayers(); l++) {

        if (network_clusterers[l]) {
            // learn prototypes for this layer
            layerInitializePrototypes(initializer, *network_tspools[l], *network_clusterers[l], training_events);

            // train
            network_clusterers[l]->toggleLearning(true);
            process(network.getLayer(l), training_events);
            network_clusterers[l]->toggleLearning(false);

        }
        // genereate events for the next layer
        training_events = cpphots::process(network.getLayer(l), training_events);

    }

}

void train(Network& network, std::vector<Events> training_events, const ClustererInitializerType& initializer, bool use_all) {

    auto network_clusterers = network.viewFull<ClustererBase>();
    auto network_tspools = network.viewFull<TimeSurfacePoolBase>();

    for (size_t l = 0; l < network.getNumLayers(); l++) {

        if (network_clusterers[l]) {
            // learn prototypes for this layer
            if (use_all)
                layerInitializePrototypes(initializer, *network_tspools[l], *network_clusterers[l], training_events);
            else
                layerInitializePrototypes(initializer, *network_tspools[l], *network_clusterers[l], training_events[0]);

            // train
            network_clusterers[l]->toggleLearning(true);
            process(network.getLayer(l), training_events);
            network_clusterers[l]->toggleLearning(false);

        }
        // genereate events for the next layer
        training_events = process(network.getLayer(l), training_events);

    }

}

}
