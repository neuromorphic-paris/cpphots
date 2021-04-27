#include "cpphots/run.h"

#include "cpphots/events_utils.h"


namespace cpphots {

Events train(Network& network, Events training_events, const ClustererInitializerType& initializer, bool skip_check) {

    auto network_clusterers = network.viewFull<ClustererBase>();
    auto network_tspools = network.viewFull<TimeSurfacePool>();

    for (size_t l = 0; l < network.getNumLayers(); l++) {

        if (network_clusterers[l]) {
            // learn prototypes for this layer
            layerInitializePrototypes(initializer, *network_tspools[l], *network_clusterers[l], training_events, !skip_check);

            // train
            network_clusterers[l]->toggleLearning(true);
            process(network.getLayer(l), training_events, skip_check);
            network_clusterers[l]->toggleLearning(false);

        }
        // genereate events for the next layer
        training_events = cpphots::process(network.getLayer(l), training_events, skip_check);

    }

    return training_events;

}

std::vector<Events> train(Network& network, std::vector<Events> training_events, const ClustererInitializerType& initializer, bool use_all, bool skip_check) {

    auto network_clusterers = network.viewFull<ClustererBase>();
    auto network_tspools = network.viewFull<TimeSurfacePool>();

    for (size_t l = 0; l < network.getNumLayers(); l++) {

        if (network_clusterers[l]) {
            // learn prototypes for this layer
            if (use_all)
                layerInitializePrototypes(initializer, *network_tspools[l], *network_clusterers[l], training_events, !skip_check);
            else
                layerInitializePrototypes(initializer, *network_tspools[l], *network_clusterers[l], training_events[0], !skip_check);

            // train
            network_clusterers[l]->toggleLearning(true);
            process(network.getLayer(l), training_events, skip_check);
            network_clusterers[l]->toggleLearning(false);

        }
        // genereate events for the next layer
        training_events = process(network.getLayer(l), training_events, skip_check);

    }

    return training_events;

}

}
