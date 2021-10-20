#include "cpphots/clustering/cosine.h"

#include "cpphots/assert.h"

namespace cpphots {


CosineClusterer::CosineClusterer() {}

CosineClusterer::CosineClusterer(uint16_t clusters, TimeSurfaceScalarType homeostasis)
    :clusters(clusters), homeostasis(homeostasis) {

    if (homeostasis > 0) {
        throw std::invalid_argument("Homeostatic regulation parameters should be < 0");
    }

    reset();

    tot_centroids_activations = 0;

}

uint16_t CosineClusterer::cluster(const TimeSurfaceType& surface) {

    cpphots_assert(hasCentroids());

    // find closest kernel
    uint16_t k = -1;
    TimeSurfaceScalarType mindist = std::numeric_limits<TimeSurfaceScalarType>::max();
    for (uint i = 0; i < centroids.size(); i++) {
        TimeSurfaceScalarType d = (surface - centroids[i]).matrix().norm();
        if (learning && tot_centroids_activations > 0) {
            d /= std::exp(homeostasis * ((TimeSurfaceScalarType)centroids_activations[i] / tot_centroids_activations * clusters - 1));
        }
        if (d < mindist) {
            mindist = d;
            k = i;
        }
    }

    cpphots_assert(k != -1);

    // update histogram
    updateHistogram(k);

    if (learning) {

        // increase kernel activation
        centroids_activations[k]++;
        tot_centroids_activations++;

        // beta
        TimeSurfaceScalarType beta = centroids[k].cwiseProduct(surface).sum() / centroids[k].matrix().norm() / surface.matrix().norm();

        // alpha
        TimeSurfaceScalarType alpha = 1. / (1. + centroids_activations[k]);

        // update kernel
        centroids[k] += alpha * beta * (surface - centroids[k]);

    }

    return k;

}

uint16_t CosineClusterer::getNumClusters() const {
    return clusters;
}

const std::vector<TimeSurfaceType>& CosineClusterer::getCentroids() const {
    return centroids;
}

bool CosineClusterer::toggleLearning(bool enable) {
    bool prev = learning;
    learning = enable;
    return prev;
}

void CosineClusterer::clearCentroids() {
    centroids.clear();
    centroids_activations.clear();
    tot_centroids_activations = 0;
}

void CosineClusterer::addCentroid(const TimeSurfaceType& centroid) {
    if (hasCentroids()) {
        throw std::runtime_error("Trying to add a centroid to a clusterer that aleady has enough.");
    }
    centroids.push_back(centroid);
    centroids_activations.push_back(0);
}

bool CosineClusterer::hasCentroids() const {
    return (centroids.size() == clusters) && (centroids_activations.size() == clusters);
}

void CosineClusterer::toStream(std::ostream& out) const {

    writeMetacommand(out, "COSINECLUSTERER");

    out << clusters << " ";
    out << learning << " ";

    out << centroids.size() << " ";
    out << centroids[0].rows() << " ";
    out << centroids[0].cols() << " ";

    out << homeostasis << " ";
    out << tot_centroids_activations << " ";

    for (const auto& pa : centroids_activations) {
        out << pa << " ";
    }
    out << "\n";
    for (const auto& p : centroids) {
        out << p << "\n";
    }

}

void CosineClusterer::fromStream(std::istream& in) {

    matchMetacommandOptional(in, "COSINECLUSTERER");

    in >> clusters;
    in >> learning;

    size_t n_centroids;
    uint16_t wx, wy;
    in >> n_centroids;
    in >> wy;
    in >> wx;

    in >> homeostasis;
    in >> tot_centroids_activations;

    centroids_activations.clear();
    centroids_activations.resize(n_centroids);
    for (auto& pa : centroids_activations) {
        in >> pa;
    }
    centroids.clear();
    for (size_t i = 0; i < n_centroids; i++) {
        TimeSurfaceType p = TimeSurfaceType::Zero(wy, wx);
        for (uint16_t y = 0; y < wy; y++) {
            for (uint16_t x = 0; x < wx; x++) {
                in >> p(y, x);
            }
        }
        centroids.push_back(p);
    }

    reset();

}

}