#include "cpphots/run.h"

#include <iostream>

#include "cpphots/events_utils.h"


namespace cpphots {

Features process_file(Network& network, const std::string& filename) {

    std::unordered_map<bool, uint16_t> change_polarities{{false, 0}, {true, 1}};
    if (network.getInputPolarities() == 1) {
        change_polarities[true] = 0;
    }

    // load file
    auto events = loadFromFile(filename, change_polarities);

    // run network
    network.resetLayers();
    for (const auto& ev : events) {
        network.process(ev);
    }

    auto feats = network.getLastHistogram();

    return feats;

}

void train_oneshot(Network& network, const std::vector<Events>& training_events, const LayerInitializer& initializer, bool use_all) {

    // prototypes initialization
    std::vector<Events> init_events = training_events;
    for (unsigned int l = 0; l < network.getNumLayers(); l++) {

        // learn prototypes for this layer
        if (use_all)
            initializer.initializePrototypes(network.getLayer(l), training_events);
        else
            initializer.initializePrototypes(network.getLayer(l), training_events[0]);

        // generate events for the next layer
        init_events = network.getLayer(l).process(init_events);

    }

    // learning
    network.toggleLearningAll(true);
    network.process(training_events);
    network.toggleLearningAll(false);

}

void train_sequential(Network& network, const std::vector<Events>& training_events, const LayerInitializer& initializer, bool use_all) {

    std::vector<Events> _training_events = training_events;

    for (unsigned int l = 0; l < network.getNumLayers(); l++) {

        // learn prototypes for this layer
        if (use_all)
            initializer.initializePrototypes(network.getLayer(l), _training_events);
        else
            initializer.initializePrototypes(network.getLayer(l), _training_events[0]);

        network.toggleLearningLayer(l, true);
        network.getLayer(l).process(_training_events);

        // genereate events for the next layer
        network.toggleLearningAll(false);
        _training_events = network.getLayer(l).process(_training_events);

    }

}

void train_oneshot(Network& network, const std::vector<std::string>& training_set, const LayerInitializer& initializer, bool use_all) {

    std::unordered_map<bool, uint16_t> change_polarities{{false, 0}, {true, 1}};
    if (network.getInputPolarities() == 1) {
        change_polarities[true] = 0;
    }

    // load all training set
    std::vector<Events> training_events;
    for (auto& filename : training_set) {
        training_events.push_back(loadFromFile(filename, change_polarities));
    }

    train_oneshot(network, training_events, initializer, use_all);

}


void train_sequential(Network& network, const std::vector<std::string>& training_set, const LayerInitializer& initializer, bool use_all) {

    std::unordered_map<bool, uint16_t> change_polarities{{false, 0}, {true, 1}};
    if (network.getInputPolarities() == 1) {
        change_polarities[true] = 0;
    }

    // load all traning set
    std::vector<Events> training_events;
    for (auto& filename : training_set) {
        training_events.push_back(loadFromFile(filename, change_polarities));
    }

    train_sequential(network, training_events, initializer, use_all);

}

double compute_accuracy(Network& network, const Classifier& classifier, const std::vector<std::pair<std::string, std::string>>& test_set) {

    network.toggleLearningAll(false);

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

    network.toggleLearningAll(false);

    std::vector<double> acc(classifiers.size());

    for (auto sample : test_set) {

        auto feats = process_file(network, sample.first);

        // classify
        for (unsigned int i = 0; i < classifiers.size(); i ++) {
             auto predicted = classifiers[i]->classifyName(feats);

            if (predicted == sample.second) {
                acc[i]++;
            }

        }

    }

    for (unsigned int i = 0; i < classifiers.size(); i ++) {
        acc[i] = acc[i] / test_set.size();
    }


    return acc;

}

}