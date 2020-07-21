#include "run.h"

#include <iostream>

#include "events_utils.h"


namespace cpphots {

Features process_file(Network& network, const std::string& filename) {

    bool mergePolarities = network.getInputPolarities() == 1;

    // std::cout << "Learning on file " << filename << std::endl;

    // load file
    auto events = loadFromFile(filename, mergePolarities);

    // run network
    network.resetLayers();
    for (const auto& ev : events) {
        network.process(ev);
    }

    auto feats = network.getLastHistogram();
    // for (auto f : feats) {
    //     std::cout << f << " ";
    // }
    // std::cout << std::endl;

    // auto prototypes = network.getLayer(2).getPrototypes();

    // std::cout << prototypes.size() << std::endl;

    // for (auto& k : prototypes) {
    //     std::cout << k << "\n\n";
    // }

    return feats;

}


void train_oneshot(Network& network, const std::vector<std::string>& training_set, const LayerInitializer& initializer, bool use_all) {

    bool mergePolarities = network.getInputPolarities() == 1;

    // load all training set
    std::vector<Events> training_events;
    for (auto& filename : training_set) {
        training_events.push_back(loadFromFile(filename, mergePolarities));
    }

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


void train_sequential(Network& network, const std::vector<std::string>& training_set, const LayerInitializer& initializer, bool use_all) {

    bool mergePolarities = network.getInputPolarities() == 1;

    // load all traning set
    std::vector<Events> training_events;
    for (auto& filename : training_set) {
        training_events.push_back(loadFromFile(filename, mergePolarities));
    }

    for (unsigned int l = 0; l < network.getNumLayers(); l++) {

        // learn prototypes for this layer
        if (use_all)
            initializer.initializePrototypes(network.getLayer(l), training_events);
        else
            initializer.initializePrototypes(network.getLayer(l), training_events[0]);

        network.toggleLearningLayer(l, true);
        network.getLayer(l).process(training_events);
        // for (auto& events : training_events) {

        //     // we process the same events two times, once to learn
        //     network.resetLayers();
        //     for (auto& ev : events) {
        //         network.getLayer(l).process(ev);
        //     }

        // }

        // and once to genereate events for the next layer
        network.toggleLearningAll(false);
        training_events = network.getLayer(l).process(training_events);

    }

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
        // std::cout << feats << std::endl;
        
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