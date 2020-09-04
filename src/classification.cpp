#include "cpphots/classification.h"

#include <stdexcept>
#include <limits>
#include <cmath>
#include <numeric>
#include <iomanip>


std::ostream& operator<<(std::ostream& out, const cpphots::Features& feats) {

    if (feats.empty()) {
        out << "[]";
        return out;
    }

    out << "[" << std::setw(3) << std::setfill('0') << feats[0];
    for (unsigned int i = 1; i < feats.size(); i++) {
        out << " " << std::setw(3) << std::setfill('0') << feats[i];
    }
    out << "]";

    return out;

}


namespace cpphots {

Classifier::Classifier(unsigned int n_classes) {

    class_feats.resize(n_classes);

}

Classifier::Classifier(const std::vector<std::string>& classes)
    :class_names(classes) {
    
    class_feats.resize(classes.size());

    for (unsigned int i = 0; i < classes.size(); i++) {
        reverse_class_names.insert({classes[i], i});
    }

}

Classifier::Classifier(Classifier* other)
    :class_feats(other->class_feats), class_names(other->class_names), reverse_class_names(other->reverse_class_names) {}

void Classifier::setClassFeatures(unsigned int cid, const Features& feats) {
    class_feats[cid] = feats;
}

void Classifier::setClassFeatures(const std::string& cname, const Features& feats) {
    unsigned int cid = reverse_class_names.at(cname);
    setClassFeatures(cid, feats);
}

unsigned int Classifier::classifyID(const Features& feats) const {

    unsigned int argmin = class_feats.size();
    double mindist = std::numeric_limits<double>::max();
    for (unsigned int i = 0; i < class_feats.size(); i++) {
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

    unsigned int cid = classifyID(feats);

    return class_names[cid];

}


double StandardClassifier::computeDistance(const Features& f1, const Features& f2) const {

    if (f1.size() != f2.size()) {
        throw std::runtime_error("Features must have the same size");
    }

    double dist = 0.0;
    for (unsigned int i = 0; i < f1.size(); i++) {
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
    for (unsigned int i = 0; i < f1.size(); i++) {
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
    for (unsigned int i = 0; i < f1.size(); i++) {
        dist += std::sqrt(f1[i] / card1 * f2[i] / card2);
    }

    return -std::log(dist);

}

}