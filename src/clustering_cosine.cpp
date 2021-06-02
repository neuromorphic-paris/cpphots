#include "cpphots/clustering_cosine.h"


namespace cpphots {


CosineClusterer::CosineClusterer() {}

CosineClusterer::CosineClusterer(uint16_t clusters)
    :clusters(clusters) {

    reset();

}

uint16_t CosineClusterer::cluster(const TimeSurfaceType& surface) {

    if (!isInitialized()) {
        throw std::runtime_error("Cannot process event: CosineClusterer is not initialized.");
    }

    // find closest kernel
    uint16_t k = -1;
    TimeSurfaceScalarType mindist = std::numeric_limits<TimeSurfaceScalarType>::max();
    for (uint i = 0; i < prototypes.size(); i++) {
        TimeSurfaceScalarType d = (surface - prototypes[i]).matrix().norm();
        if (d < mindist) {
            mindist = d;
            k = i;
        }
    }

    // update histogram
    updateHistogram(k);

    if (learning) {

        // increase kernel activation
        prototypes_activations[k]++;

        // beta
        TimeSurfaceScalarType beta = prototypes[k].cwiseProduct(surface).sum() / prototypes[k].matrix().norm() / surface.matrix().norm();

        // alpha
        TimeSurfaceScalarType alpha = 1. / (1. + prototypes_activations[k]);

        // update kernel
        prototypes[k] += alpha * beta * (surface - prototypes[k]);

    }

    return k;

}

uint16_t CosineClusterer::getNumClusters() const {
    return clusters;
}

std::vector<TimeSurfaceType> CosineClusterer::getPrototypes() const {
    return prototypes;
}

bool CosineClusterer::toggleLearning(bool enable) {
    bool prev = learning;
    learning = enable;
    return prev;
}

void CosineClusterer::clearPrototypes() {
    prototypes.clear();
    prototypes_activations.clear();
}

void CosineClusterer::addPrototype(const TimeSurfaceType& proto) {
    if (isInitialized()) {
        throw std::runtime_error("Trying to add a prototype to an already initialized Layer.");
    }
    prototypes.push_back(proto);
    prototypes_activations.push_back(0);
}

bool CosineClusterer::isInitialized() const {
    return (prototypes.size() == clusters) && (prototypes_activations.size() == clusters);
}

void CosineClusterer::toStream(std::ostream& out) const {

    writeMetacommand(out, "COSINECLUSTERER");

    out << clusters << " ";
    out << learning << " ";

    out << prototypes.size() << " ";
    out << prototypes[0].rows() << " ";
    out << prototypes[0].cols() << " ";

    for (const auto& pa : prototypes_activations) {
        out << pa << " ";
    }
    out << "\n";
    for (const auto& p : prototypes) {
        out << p << "\n";
    }

}

void CosineClusterer::fromStream(std::istream& in) {

    matchMetacommandOptional(in, "COSINECLUSTERER");

    in >> clusters;
    in >> learning;

    size_t n_prototypes;
    uint16_t wx, wy;
    in >> n_prototypes;
    in >> wy;
    in >> wx;

    prototypes_activations.clear();
    prototypes_activations.resize(n_prototypes);
    for (auto& pa : prototypes_activations) {
        in >> pa;
    }
    prototypes.clear();
    for (size_t i = 0; i < n_prototypes; i++) {
        TimeSurfaceType p = TimeSurfaceType::Zero(wy, wx);
        for (uint16_t y = 0; y < wy; y++) {
            for (uint16_t x = 0; x < wx; x++) {
                in >> p(y, x);
            }
        }
        prototypes.push_back(p);
    }

    reset();

}

}