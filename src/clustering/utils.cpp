#include "cpphots/clustering/utils.h"

#include <random>
#include <set>
#include <ctime>
#include <functional>


namespace cpphots {

std::vector<uint32_t> ClustererHistogramMixin::getHistogram() const {
    return hist;
}

void ClustererHistogramMixin::reset() {
    hist.clear();
    hist.resize(getNumClusters());
}

void ClustererHistogramMixin::updateHistogram(uint16_t k) {
    hist[k]++;
}


bool ClustererOnlineMixin::isOnline() const {
    return true;
}

void ClustererOnlineMixin::train(const std::vector<TimeSurfaceType>& tss) {
    toggleLearning(true);
    for (const auto& ts : tss) {
        cluster(ts);
    }
    toggleLearning(false);
}


uint16_t ClustererOfflineMixin::cluster(const TimeSurfaceType& surface) {
    if (learning) {
        learning_tss.push_back(surface);
    }
    return 0;
}

bool ClustererOfflineMixin::isOnline() const {
    return false;
}

bool ClustererOfflineMixin::toggleLearning(bool enable) {

    bool prev = learning;
    learning = enable;

    if (!learning) {
        train(learning_tss);
    }

    learning_tss.clear();

    return prev;

}

bool ClustererOfflineMixin::isLearning() const {
    return learning;
}


void ClustererUniformSeeding(interfaces::Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces) {

    std::vector<TimeSurfaceType> selected;
    std::sample(time_surfaces.begin(), time_surfaces.end(), std::back_inserter(selected), clusterer.getNumClusters(), std::mt19937{std::random_device{}()});

    for (auto& c : selected) {
        clusterer.addCentroid(c);
    }

}

void ClustererPlusPlusSeeding(interfaces::Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces) {

    // chosen surfaces
    std::set<int> chosen;
    std::vector<TimeSurfaceType> centroids;

    // choose first time surface at random
    std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<> idist(0, time_surfaces.size()-1);
    int first = idist(gen);
    centroids.push_back(time_surfaces[first]);
    chosen.insert(first);

    std::vector<TimeSurfaceScalarType> distances(time_surfaces.size());
    TimeSurfaceScalarType distsum = 0.0;

    for (size_t k = 1; k < clusterer.getNumClusters(); k++) {

        distsum = 0.0;

        // compute all squared distances
        for (size_t ts = 0; ts < time_surfaces.size(); ts++) {

            TimeSurfaceScalarType mindist = std::numeric_limits<TimeSurfaceScalarType>::max();
            for (const auto& c : centroids) {
                TimeSurfaceScalarType d = (c - time_surfaces[ts]).matrix().squaredNorm();
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
                centroids.push_back(time_surfaces[ts]);
                chosen.insert(ts);
                break;
            }
            currdist += distances[ts];
        }

    }

    if (chosen.size() != clusterer.getNumClusters()) {
        throw std::runtime_error("Something went wrong with the plusplus seeding.");
    }

    for (const auto& c : centroids) {
        clusterer.addCentroid(c);
    }

}

void ClustererAFKMC2SeedingImpl(interfaces::Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces, uint16_t chain) {

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
        clusterer.addCentroid(c);
    }

}

ClustererSeedingType ClustererAFKMC2Seeding(uint16_t chain) {

    return std::bind(ClustererAFKMC2SeedingImpl, std::placeholders::_1, std::placeholders::_2, chain);

}

void ClustererRandomSeedingImpl(interfaces::Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces, uint16_t width, uint16_t height) {

    std::srand((unsigned int) std::time(0));

    for (uint16_t i = 0; i < clusterer.getNumClusters(); i++) {
        clusterer.addCentroid((TimeSurfaceType::Random(height, width) + 1.f) /2.f);
    }

}

ClustererSeedingType ClustererRandomSeeding(uint16_t width, uint16_t height) {

    return std::bind(ClustererRandomSeedingImpl, std::placeholders::_1, std::placeholders::_2, width, height);

}

}