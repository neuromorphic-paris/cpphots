#include "cpphots/run.h"

#include <iostream>

#include "cpphots/events_utils.h"


namespace cpphots {

Features process_file(Network& network, const std::string& filename) {

    // load file
    auto events = loadFromFile(filename);

    // run network
    network.reset();
    for (const auto& ev : events) {
        network.process(ev);
    }

    auto feats = network.back<Clusterer>().getHistogram();

    return feats;

}

void train_oneshot(Network& network, const std::vector<Events>& training_events, const ClustererInitializerType& initializer, bool use_all) {

    auto network_clusterers = network.viewFull<Clusterer>();
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
    for (auto cl : network.view<Clusterer>()) {
        cl->toggleLearning(true);
    }
    cpphots::process(network, training_events);
    for (auto cl : network.view<Clusterer>()) {
        cl->toggleLearning(false);
    }

}

void train_sequential(Network& network, const std::vector<Events>& training_events, const ClustererInitializerType& initializer, bool use_all) {

    auto network_clusterers = network.viewFull<Clusterer>();
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

void train_oneshot(Network& network, const std::vector<std::string>& training_set, const ClustererInitializerType& initializer, bool use_all) {

    // load all training set
    std::vector<Events> training_events;
    for (auto& filename : training_set) {
        training_events.push_back(loadFromFile(filename));
    }

    train_oneshot(network, training_events, initializer, use_all);

}


void train_sequential(Network& network, const std::vector<std::string>& training_set, const ClustererInitializerType& initializer, bool use_all) {

    // load all traning set
    std::vector<Events> training_events;
    for (auto& filename : training_set) {
        training_events.push_back(loadFromFile(filename));
    }

    train_sequential(network, training_events, initializer, use_all);

}

double compute_accuracy(Network& network, const Classifier& classifier, const std::vector<std::pair<std::string, std::string>>& test_set) {

    for (auto cl : network.view<Clusterer>()) {
        cl->toggleLearning(false);
    }

    double acc = 0;

    for (auto sample : test_set) {

        // classify
        auto predicted = classifier.classifyName(process_file(network, sample.first));

        if (predicted == sample.second) {
            acc++;
        }

    }

    return acc / test_set.size();

}

std::vector<double> compute_accuracy(Network& network, const std::vector<Classifier*>& classifiers, const std::vector<std::pair<std::string, std::string>>& test_set) {

    for (auto cl : network.view<Clusterer>()) {
        cl->toggleLearning(false);
    }

    std::vector<double> acc(classifiers.size());

    for (auto sample : test_set) {

        auto feats = process_file(network, sample.first);

        // classify
        for (size_t i = 0; i < classifiers.size(); i ++) {
             auto predicted = classifiers[i]->classifyName(feats);

            if (predicted == sample.second) {
                acc[i]++;
            }

        }

    }

    for (size_t i = 0; i < classifiers.size(); i ++) {
        acc[i] = acc[i] / test_set.size();
    }


    return acc;

}

}