#include "cpphots/classification.h"

#include <stdexcept>
#include <limits>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <algorithm>

namespace cpphots {

std::ostream& operator<<(std::ostream& out, const cpphots::Features& feats) {

    if (feats.empty()) {
        out << "[]";
        return out;
    }

    auto maxval = std::max_element(feats.begin(), feats.end());
    int w = std::to_string(*maxval).size();

    out << "[" << std::setw(w) << std::setfill(' ') << feats[0];
    for (size_t i = 1; i < feats.size(); i++) {
        out << " " << std::setw(w) << std::setfill(' ') << feats[i];
    }
    out << "]";

    return out;

}


double features_quality(const Features& feats) {

    double q = 0;
    for (auto& f : feats) {
        if (f > 0)
            q++;
    }

    return q / feats.size();

}


Classifier::Classifier(size_t n_classes) {

    class_feats.resize(n_classes);

}

Classifier::Classifier(const std::vector<std::string>& classes)
    :class_names(classes) {

    class_feats.resize(classes.size());

    for (size_t i = 0; i < classes.size(); i++) {
        reverse_class_names.insert({classes[i], i});
    }

}

Classifier::Classifier(Classifier* other)
    :class_feats(other->class_feats), class_names(other->class_names), reverse_class_names(other->reverse_class_names) {}

void Classifier::setClassFeatures(size_t cid, const Features& feats) {
    class_feats[cid] = feats;
}

void Classifier::setClassFeatures(const std::string& cname, const Features& feats) {
    size_t cid = reverse_class_names.at(cname);
    setClassFeatures(cid, feats);
}

size_t Classifier::classifyID(const Features& feats) const {

    size_t argmin = class_feats.size();
    double mindist = std::numeric_limits<double>::max();
    for (size_t i = 0; i < class_feats.size(); i++) {
        double d = computeDistance(class_feats[i], feats);
        if (d < mindist) {
            mindist = d;
            argmin = i;
        }
    }

    return argmin;

}

std::string Classifier::classifyName(const Features& feats) const {

    if (class_names.empty())
        throw std::runtime_error("Cannot output class name if no names were set at construction time");

    size_t cid = classifyID(feats);

    return class_names[cid];

}


double StandardClassifier::computeDistance(const Features& f1, const Features& f2) const {

    if (f1.size() != f2.size()) {
        throw std::runtime_error("Features must have the same size");
    }

    double dist = 0.0;
    for (size_t i = 0; i < f1.size(); i++) {
        dist += std::pow(double(f1[i]) - double(f2[i]), 2);
    }

    return std::sqrt(dist);

}


double NormalizedClassifier::computeDistance(const Features& f1, const Features& f2) const {

    if (f1.size() != f2.size()) {
        throw std::runtime_error("Features must have the same size");
    }

    double card1 = std::accumulate(f1.begin(), f1.end(), 0.0);
    double card2 = std::accumulate(f2.begin(), f2.end(), 0.0);

    double dist = 0.0;
    for (size_t i = 0; i < f1.size(); i++) {
        dist += std::pow(f1[i] / card1 - f2[i] / card2, 2);
    }

    return std::sqrt(dist);

}


double BhattacharyyaClassifier::computeDistance(const Features& f1, const Features& f2) const {

    if (f1.size() != f2.size()) {
        throw std::runtime_error("Features must have the same size");
    }

    double card1 = std::accumulate(f1.begin(), f1.end(), 0.0);
    double card2 = std::accumulate(f2.begin(), f2.end(), 0.0);

    double dist = 0.0;
    for (size_t i = 0; i < f1.size(); i++) {
        dist += std::sqrt(f1[i] / card1 * f2[i] / card2);
    }

    return -std::log(dist);

}

}