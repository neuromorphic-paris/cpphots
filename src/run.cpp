#include "cpphots/run.h"

#include "cpphots/events_utils.h"
#include "cpphots/interfaces/time_surface.h"
#include "cpphots/interfaces/clustering.h"


namespace cpphots {

Events train(Network& network, Events training_events, const ClustererSeedingType& seeding, bool skip_check) {

    for (size_t l = 0; l < network.getNumLayers(); l++) {

        Layer& layer = network[l];

        if (layer.canCluster()) {

            // seed centroids for this layer
            layerSeedCentroids(seeding, layer, training_events, !skip_check);

            // train
            layer.toggleLearning(true);
            process(layer, training_events, skip_check);
            layer.toggleLearning(false);

        }

        // genereate events for the next layer
        training_events = cpphots::process(layer, training_events, skip_check);

    }

    return training_events;

}

std::vector<Events> train(Network& network, std::vector<Events> training_events, const ClustererSeedingType& seeding, bool use_all, bool skip_check) {

    for (size_t l = 0; l < network.getNumLayers(); l++) {

        Layer& layer = network[l];

        if (layer.canCluster()) {

            // seed centroids for this layer
            if (use_all)
                layerSeedCentroids(seeding, layer, training_events, !skip_check);
            else
                layerSeedCentroids(seeding, layer, training_events[0], !skip_check);

            // train
            layer.toggleLearning(true);
            process(layer, training_events, skip_check);
            layer.toggleLearning(false);

        }

        // genereate events for the next layer
        training_events = process(layer, training_events, skip_check);

    }

    return training_events;

}

}
