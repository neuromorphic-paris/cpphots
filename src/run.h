#ifndef TESTING_H
#define TESTING_H

#include <vector>
#include <string>
#include <utility>

#include "layer.h"
#include "network.h"
#include "classification.h"


namespace cpphots {

Features process_file(Network& network, const std::string& filename);

// training
void train_oneshot(Network& network, const std::vector<std::string>& training_set, const LayerInitializer& initializer, bool use_all = true);
void train_sequential(Network& network, const std::vector<std::string>& training_set, const LayerInitializer& initializer, bool use_all = true);

// test suite
// takes a trained network, a trained classifier and a test set (vector<filename, label>)
double compute_accuracy(Network& network, const Classifier& classifier, const std::vector<std::pair<std::string, std::string>>& test_set);
std::vector<double> compute_accuracy(Network& network, const std::vector<Classifier*>& classifiers, const std::vector<std::pair<std::string, std::string>>& test_set);

}

#endif