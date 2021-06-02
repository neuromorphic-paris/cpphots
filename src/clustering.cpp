#include "cpphots/clustering.h"

#include <random>
#include <set>
#include <ctime>
#include <functional>


namespace cpphots {


HOTSClusterer::HOTSClusterer() {}

HOTSClusterer::HOTSClusterer(uint16_t clusters)
    :clusters(clusters) {

    reset();

}

uint16_t HOTSClusterer::cluster(const TimeSurfaceType& surface) {

    if (!isInitialized()) {
        throw std::runtime_error("Cannot process event: HOTSClusterer is not initialized.");
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

uint16_t HOTSClusterer::getNumClusters() const {
    return clusters;
}

std::vector<TimeSurfaceType> HOTSClusterer::getPrototypes() const {
    return prototypes;
}

bool HOTSClusterer::toggleLearning(bool enable) {
    bool prev = learning;
    learning = enable;
    return prev;
}

void HOTSClusterer::clearPrototypes() {
    prototypes.clear();
    prototypes_activations.clear();
}

void HOTSClusterer::addPrototype(const TimeSurfaceType& proto) {
    if (isInitialized()) {
        throw std::runtime_error("Trying to add a prototype to an already initialized Layer.");
    }
    prototypes.push_back(proto);
    prototypes_activations.push_back(0);
}

bool HOTSClusterer::isInitialized() const {
    return (prototypes.size() == clusters) && (prototypes_activations.size() == clusters);
}

void HOTSClusterer::toStream(std::ostream& out) const {

    writeMetacommand(out, "HOTSCLUSTERER");

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

void HOTSClusterer::fromStream(std::istream& in) {

    matchMetacommandOptional(in, "HOTSCLUSTERER");

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


void ClustererUniformInitializer(interfaces::Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces) {

    std::vector<TimeSurfaceType> selected;
    std::sample(time_surfaces.begin(), time_surfaces.end(), std::back_inserter(selected), clusterer.getNumClusters(), std::mt19937{std::random_device{}()});

    for (auto& p : selected) {
        clusterer.addPrototype(p);
    }

}

void ClustererPlusPlusInitializer(interfaces::Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces) {

    // chosen surfaces
    std::set<int> chosen;
    std::vector<TimeSurfaceType> prototypes;

    // choose first time surface at random
    std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<> idist(0, time_surfaces.size()-1);
    int first = idist(gen);
    prototypes.push_back(time_surfaces[first]);
    chosen.insert(first);

    std::vector<TimeSurfaceScalarType> distances(time_surfaces.size());
    TimeSurfaceScalarType distsum = 0.0;

    for (size_t k = 1; k < clusterer.getNumClusters(); k++) {

        distsum = 0.0;

        // compute all squared distances
        for (size_t ts = 0; ts < time_surfaces.size(); ts++) {

            TimeSurfaceScalarType mindist = std::numeric_limits<TimeSurfaceScalarType>::max();
            for (const auto& p : prototypes) {
                TimeSurfaceScalarType d = (p - time_surfaces[ts]).matrix().squaredNorm();
                if (d < mindist)
                    mindist = d;
            }

            distances[ts] = mindist;
            distsum += mindist;

        }

        // choose random surface, based on distances
        std::uniform_real_distribution<TimeSurfaceScalarType> rdist(0.0, distsum);
        TimeSurfaceScalarType x = rdist(gen);
        TimeSurfaceScalarType currdist = 0.0;

        for (size_t ts = 0; ts < time_surfaces.size(); ts++) {
            if (x < currdist + distances[ts]) {
                prototypes.push_back(time_surfaces[ts]);
                chosen.insert(ts);
                break;
            }
            currdist += distances[ts];
        }

    }

    if (chosen.size() != clusterer.getNumClusters()) {
        throw std::runtime_error("Something went wrong with the plusplus initialization.");
    }

    for (const auto& p : prototypes) {
        clusterer.addPrototype(p);
    }

}

void ClustererAFKMC2InitializerImpl(interfaces::Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces, uint16_t chain) {

    std::mt19937 mt{std::random_device{}()};

    int N = time_surfaces.size();
    int M = clusterer.getNumClusters();

    std::vector<cpphots::TimeSurfaceType> centroids(M);

    // draw first cluster
    std::uniform_int_distribution<int> initial(0, N-1);
    int first_cluster = initial(mt);
    centroids[0] = time_surfaces[first_cluster];

    // compute proposal distribution
    std::vector<TimeSurfaceScalarType> q(N);

    for (int n = 0; n < N; n++) {
        q[n] = (time_surfaces[n] - centroids[0]).matrix().squaredNorm();
    }

    TimeSurfaceScalarType dsum = std::accumulate(q.begin(), q.end(), 0.0);
    TimeSurfaceScalarType wsum = 1.0 * N;

    for (int n = 0; n < N; n++) {
        q[n] = 0.5 * (q[n] / dsum + 1.0 / wsum);
    }

    std::discrete_distribution<int> draw_q(q.begin(), q.end());
    std::uniform_real_distribution<TimeSurfaceScalarType> uniform(0.0, 1.0);

    for (int h = 0; h < M; h++) {

        // initialize a new Markov chain
        int data_idx = draw_q(mt);

        // compute distance to closest cluster
        TimeSurfaceScalarType dist = std::numeric_limits<TimeSurfaceScalarType>::max();
        for (int _h = 0; _h < h; _h++) {
            dist = std::min(dist, (time_surfaces[data_idx] - centroids[_h]).matrix().squaredNorm());
        }
        TimeSurfaceScalarType data_key = dist;

        // Markov chain
        for (int i = 1; i < chain; i++) {

            // draw new potential cluster center from proposal distribution
            int y_idx = draw_q(mt);

            // compute distance to closest cluster
            TimeSurfaceScalarType dist = std::numeric_limits<TimeSurfaceScalarType>::max();
            for (int _h = 0; _h < h; _h++) {
                dist = std::min(dist, (time_surfaces[y_idx] - centroids[_h]).matrix().squaredNorm());
            }
            TimeSurfaceScalarType y_key = dist;
            
            // determine the probability to accept the new sample y_idx
            TimeSurfaceScalarType y_prob = y_key / q[y_idx];
            TimeSurfaceScalarType data_prob = data_key / q[data_idx];

            if (((y_prob / data_prob) > uniform(mt)) || (data_prob == 0)) {
                data_idx = y_idx;
                data_key = y_key;
            }

        }

        centroids[h] = time_surfaces[data_idx];

    }

    for (auto& c : centroids) {
        clusterer.addPrototype(c);
    }

}

ClustererInitializerType ClustererAFKMC2Initializer(uint16_t chain) {

    return std::bind(ClustererAFKMC2InitializerImpl, std::placeholders::_1, std::placeholders::_2, chain);

}

void ClustererRandomInitializerImpl(interfaces::Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces, uint16_t width, uint16_t height) {

    std::srand((unsigned int) std::time(0));

    for (uint16_t i = 0; i < clusterer.getNumClusters(); i++) {
        clusterer.addPrototype(TimeSurfaceType::Random(height, width) + 1.f /2.f);
    }

}

ClustererInitializerType ClustererRandomInitializer(uint16_t width, uint16_t height) {

    return std::bind(ClustererRandomInitializerImpl, std::placeholders::_1, std::placeholders::_2, width, height);

}

}