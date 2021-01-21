#include "cpphots/clustering.h"

#include <random>
#include <set>
#include <ctime>
#include <functional>


namespace cpphots {

Clusterer::Clusterer() {}

Clusterer::Clusterer(uint16_t clusters)
    :clusters(clusters) {

    hist.resize(clusters);

}

uint16_t Clusterer::cluster(const TimeSurfaceType& surface) {

    if (!isInitialized()) {
        throw std::runtime_error("Cannot process event: Clusterer is not initialized.");
    }

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


void ClustererUniformInitializer(Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces) {

    std::vector<TimeSurfaceType> selected;
    std::sample(time_surfaces.begin(), time_surfaces.end(), std::back_inserter(selected), clusterer.getNumClusters(), std::mt19937{std::random_device{}()});

    for (auto& p : selected) {
        clusterer.addPrototype(p);
    }

}

void ClustererPlusPlusInitializer(Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces) {

    // chosen surfaces
    std::set<int> chosen;

    // choose first time surface at random
    std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<> idist(0, time_surfaces.size());
    int first = idist(gen);
    clusterer.addPrototype(time_surfaces[first]);
    chosen.insert(first);

    std::vector<float> distances(time_surfaces.size());
    float distsum = 0.0;

    for (size_t k = 1; k < clusterer.getNumClusters(); k++) {

        distsum = 0.0;

        // compute all squared distances
        for (size_t ts = 0; ts < time_surfaces.size(); ts++) {

            float mindist = std::numeric_limits<float>::max();
            for (const auto& p : clusterer.getPrototypes()) {
                float d = (p - time_surfaces[ts]).matrix().norm();
                d = d * d;
                if (d < mindist)
                    mindist = d;
            }

            distances[ts] = mindist;
            distsum += mindist;

        }

        // choose random surface, based on distances
        std::uniform_real_distribution<float> rdist(0.0, distsum);
        float x = rdist(gen);
        float currdist = 0.0;

        for (size_t ts = 0; ts < time_surfaces.size(); ts++) {
            if (x < currdist + distances[ts]) {
                clusterer.addPrototype(time_surfaces[ts]);
                chosen.insert(ts);
                break;
            }
            currdist += distances[ts];
        }

    }

    if (chosen.size() != clusterer.getNumClusters()) {
        throw std::runtime_error("Something went wrong with the plusplus initialization.");
    }

}

void ClustererRandomInitializerImpl(Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces, uint16_t width, uint16_t height) {

    std::srand((unsigned int) std::time(0));

    for (uint16_t i = 0; i < clusterer.getNumClusters(); i++) {
        clusterer.addPrototype(TimeSurfaceType::Random(height, width) + 1.f /2.f);
    }

}

ClustererInitializerType ClustererRandomInitializer(uint16_t width, uint16_t height) {

    return std::bind(ClustererRandomInitializerImpl, std::placeholders::_1, std::placeholders::_2, width, height);

}

}