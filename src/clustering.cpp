#include "cpphots/clustering.h"

#include <random>
#include <set>
#include <ctime>
#include <functional>


namespace cpphots {

HOTSClusterer::HOTSClusterer() {}

HOTSClusterer::HOTSClusterer(uint16_t clusters)
    :clusters(clusters) {

    hist.resize(clusters);

}

uint16_t HOTSClusterer::cluster(const TimeSurfaceType& surface) {

    if (!isInitialized()) {
        throw std::runtime_error("Cannot process event: HOTSClusterer is not initialized.");
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

std::vector<uint32_t> HOTSClusterer::getHistogram() const {
    return hist;
}

void HOTSClusterer::reset() {
    hist.clear();
    hist.resize(clusters);
}

std::ostream& operator<<(std::ostream& out, const HOTSClusterer& clusterer) {

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

std::istream& operator>>(std::istream& in, HOTSClusterer& clusterer) {

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


void ClustererUniformInitializer(ClustererBase& clusterer, const std::vector<TimeSurfaceType>& time_surfaces) {

    std::vector<TimeSurfaceType> selected;
    std::sample(time_surfaces.begin(), time_surfaces.end(), std::back_inserter(selected), clusterer.getNumClusters(), std::mt19937{std::random_device{}()});

    for (auto& p : selected) {
        clusterer.addPrototype(p);
    }

}

void ClustererPlusPlusInitializer(ClustererBase& clusterer, const std::vector<TimeSurfaceType>& time_surfaces) {

    // chosen surfaces
    std::set<int> chosen;
    std::vector<TimeSurfaceType> prototypes;

    // choose first time surface at random
    std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<> idist(0, time_surfaces.size()-1);
    int first = idist(gen);
    prototypes.push_back(time_surfaces[first]);
    chosen.insert(first);

    std::vector<float> distances(time_surfaces.size());
    float distsum = 0.0;

    for (size_t k = 1; k < clusterer.getNumClusters(); k++) {

        distsum = 0.0;

        // compute all squared distances
        for (size_t ts = 0; ts < time_surfaces.size(); ts++) {

            float mindist = std::numeric_limits<float>::max();
            for (const auto& p : prototypes) {
                float d = (p - time_surfaces[ts]).matrix().squaredNorm();
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

void ClustererAFKMC2InitializerImpl(ClustererBase& clusterer, const std::vector<TimeSurfaceType>& time_surfaces, uint16_t chain) {

    std::mt19937 mt{std::random_device{}()};

    int N = time_surfaces.size();
    int M = clusterer.getNumClusters();

    std::vector<cpphots::TimeSurfaceType> centroids(M);

    // draw first cluster
    std::uniform_int_distribution<int> initial(0, N-1);
    int first_cluster = initial(mt);
    centroids[0] = time_surfaces[first_cluster];

    // compute proposal distribution
    std::vector<float> q(N);

    for (int n = 0; n < N; n++) {
        q[n] = (time_surfaces[n] - centroids[0]).matrix().squaredNorm();
    }

    float dsum = std::accumulate(q.begin(), q.end(), 0.0);
    float wsum = 1.0 * N;

    for (int n = 0; n < N; n++) {
        q[n] = 0.5 * (q[n] / dsum + 1.0 / wsum);
    }

    std::discrete_distribution<int> draw_q(q.begin(), q.end());
    std::uniform_real_distribution<float> uniform(0.0, 1.0);

    for (int h = 0; h < M; h++) {

        // initialize a new Markov chain
        int data_idx = draw_q(mt);

        // compute distance to closest cluster
        float dist = std::numeric_limits<float>::max();
        for (int _h = 0; _h < h; _h++) {
            dist = std::min(dist, (time_surfaces[data_idx] - centroids[_h]).matrix().squaredNorm());
        }
        float data_key = dist;

        // Markov chain
        for (int i = 1; i < chain; i++) {

            // draw new potential cluster center from proposal distribution
            int y_idx = draw_q(mt);

            // compute distance to closest cluster
            float dist = std::numeric_limits<float>::max();
            for (int _h = 0; _h < h; _h++) {
                dist = std::min(dist, (time_surfaces[y_idx] - centroids[_h]).matrix().squaredNorm());
            }
            float y_key = dist;
            
            // determine the probability to accept the new sample y_idx
            float y_prob = y_key / q[y_idx];
            float data_prob = data_key / q[data_idx];

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

void ClustererRandomInitializerImpl(ClustererBase& clusterer, const std::vector<TimeSurfaceType>& time_surfaces, uint16_t width, uint16_t height) {

    std::srand((unsigned int) std::time(0));

    for (uint16_t i = 0; i < clusterer.getNumClusters(); i++) {
        clusterer.addPrototype(TimeSurfaceType::Random(height, width) + 1.f /2.f);
    }

}

ClustererInitializerType ClustererRandomInitializer(uint16_t width, uint16_t height) {

    return std::bind(ClustererRandomInitializerImpl, std::placeholders::_1, std::placeholders::_2, width, height);

}

}