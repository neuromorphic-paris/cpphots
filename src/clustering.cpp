#include "cpphots/clustering.h"

namespace cpphots {

Clusterer::Clusterer() {}

Clusterer::Clusterer(uint16_t clusters)
    :clusters(clusters) {

    hist.resize(clusters);

}

uint16_t Clusterer::cluster(const TimeSurfaceType& surface) {

    // find closest kernel
    uint16_t k = -1;
    float mindist = std::numeric_limits<float>::max();
    for (uint i = 0; i < prototypes.size(); i++) {
        float d = (surface - prototypes[i]).matrix().norm();
        if (d < mindist) {
            mindist = d;
            k = i;
        }
    }

    // update histogram
    hist[k]++;

    if (learning) {

        // increase kernel activation
        prototypes_activations[k]++;

        // beta
        float beta = prototypes[k].cwiseProduct(surface).sum() / prototypes[k].matrix().norm() / surface.matrix().norm();
        // float beta = maxcosine;

        // alpha
        float alpha = 1. / (1. + prototypes_activations[k]);

        // update kernel
        prototypes[k] += alpha * beta * (surface - prototypes[k]);

    }

    return k;

}

uint16_t Clusterer::getNumClusters() const {
    return clusters;
}

std::vector<TimeSurfaceType> Clusterer::getPrototypes() const {
    return prototypes;
}

bool Clusterer::toggleLearning(bool enable) {
    bool prev = learning;
    learning = enable;
    return prev;
}

void Clusterer::clearPrototypes() {
    prototypes.clear();
    prototypes_activations.clear();
}

void Clusterer::addPrototype(const TimeSurfaceType& proto) {
    if (isInitialized()) {
        throw std::runtime_error("Trying to add a prototype to an already initialized Layer.");
    }
    prototypes.push_back(proto);
    prototypes_activations.push_back(0);
}

std::vector<uint32_t> Clusterer::getHistogram() const {
    return hist;
}

void Clusterer::reset() {
    hist.clear();
    hist.resize(clusters);
}

std::ostream& operator<<(std::ostream& out, const Clusterer& clusterer) {

    out << clusterer.clusters << " ";
    out << clusterer.learning << " ";

    out << clusterer.prototypes.size() << " ";
    out << clusterer.prototypes[0].rows() << " ";
    out << clusterer.prototypes[0].cols() << " ";

    for (const auto& pa : clusterer.prototypes_activations) {
        out << pa << " ";
    }
    out << "\n";
    for (const auto& p : clusterer.prototypes) {
        out << p << "\n";
    }

    return out;

}

std::istream& operator>>(std::istream& in, Clusterer& clusterer) {
    
    in >> clusterer.clusters;
    in >> clusterer.learning;

    size_t n_prototypes;
    uint16_t wx, wy;
    in >> n_prototypes;
    in >> wy;
    in >> wx;

    clusterer.prototypes_activations.clear();
    clusterer.prototypes_activations.resize(n_prototypes);
    for (auto& pa : clusterer.prototypes_activations) {
        in >> pa;
    }
    clusterer.prototypes.clear();
    for (size_t i = 0; i < n_prototypes; i++) {
        TimeSurfaceType p = TimeSurfaceType::Zero(wy, wx);
        for (uint16_t y = 0; y < wy; y++) {
            for (uint16_t x = 0; x < wx; x++) {
                in >> p(y, x);
            }
        }
        clusterer.prototypes.push_back(p);
    }

    clusterer.hist.clear();
    clusterer.hist.resize(clusterer.clusters);

    return in;

}

}