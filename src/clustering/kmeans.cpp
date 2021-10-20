#include "cpphots/clustering/kmeans.h"

#include "cpphots/assert.h"


namespace cpphots {

////////////////////////
// k-means internals
using KMeansDataType = std::vector<cpphots::TimeSurfaceType>;

bool operator==(const KMeansDataType& data1, const KMeansDataType& data2) {

    if (data1.size() != data2.size()) {
        return false;
    }

    for (size_t i = 0; i < data1.size(); i++) {
        const auto& d1 = data1[i];
        const auto& d2 = data2[i];
        if (!d1.isApprox(d2)) {
            return false;
        }
    }

    return true;

}

uint16_t find_closest_centroid(const TimeSurfaceType& surface, const KMeansDataType& centroids) {

    size_t idx = -1;
    TimeSurfaceScalarType min = std::numeric_limits<TimeSurfaceScalarType>::max();

    for (size_t i = 0; i < centroids.size(); i++) {
        cpphots::TimeSurfaceScalarType d = (centroids[i] - surface).matrix().norm();
        if (d < min) {
            idx = i;
            min = d;
        }
    }

    cpphots_assert(idx != -1);

    return idx;

}


KMeansDataType kmeans(const KMeansDataType& data, KMeansDataType centroids, uint16_t k, uint16_t max_iterations) {

    KMeansDataType old_centroids;
	KMeansDataType old_old_centroids;

    std::vector<uint16_t> clusters(data.size());
    std::vector<int> count(k, 0);

    uint16_t it = 0;
    for (; it < max_iterations; it++) {

        // compute clusters
        for (size_t i = 0; i < data.size(); i++) {
            clusters[i] = find_closest_centroid(data[i], centroids);
        }

        old_old_centroids = old_centroids;
		old_centroids = centroids;

        // recompute centroids
        centroids = KMeansDataType(k, cpphots::TimeSurfaceType::Zero(data[0].rows(), data[0].cols()));
        count = std::vector<int>(k, 0);
        for (size_t i = 0; i < data.size(); i++) {
            centroids[clusters[i]] += data[i];
            count[clusters[i]]++;
        }

        for (uint16_t i = 0; i < k; i++) {
            if (count[i] == 0) {
                centroids[i] = old_centroids[i];
            } else {
                centroids[i] /= count[i];
            }
        }

        // check termination
        if (centroids == old_centroids || centroids == old_old_centroids) {
            break;
        }

    }

    return centroids;

}

// end k-means internals
////////////////////////


KMeansClusterer::KMeansClusterer() {}

KMeansClusterer::KMeansClusterer(uint16_t clusters, uint16_t max_iterations)
    :clusters(clusters), max_iterations(max_iterations) {

    reset();

}

uint16_t KMeansClusterer::cluster(const TimeSurfaceType& surface) {

    ClustererOfflineMixin::cluster(surface);

    if (isLearning()) {
        return 0;
    }

    cpphots_assert(hasCentroids());

    // find the closest centroid
    size_t idx = find_closest_centroid(surface, centroids);

    // update histogram
    updateHistogram(idx);

    return idx;

}

uint16_t KMeansClusterer::getNumClusters() const {
    return clusters;
}

void KMeansClusterer::addCentroid(const TimeSurfaceType& centroid) {
    if (hasCentroids()) {
        throw std::runtime_error("Trying to add a centroid to a clusterer that aleady has enough.");
    }
    centroids.push_back(centroid);
}

const std::vector<TimeSurfaceType>& KMeansClusterer::getCentroids() const {
    return centroids;
}

void KMeansClusterer::clearCentroids() {
    centroids.clear();
}

bool KMeansClusterer::hasCentroids() const {
    return centroids.size() == clusters;
}

void KMeansClusterer::train(const std::vector<TimeSurfaceType>& tss) {

    cpphots_assert(hasCentroids());

    centroids = kmeans(tss, centroids, clusters, max_iterations);

}

void KMeansClusterer::toStream(std::ostream& out) const {

    writeMetacommand(out, "KMEANSCLUSTERER");

    out << clusters << " ";
    out << max_iterations << " ";

    out << centroids.size() << " ";
    out << centroids[0].rows() << " ";
    out << centroids[0].cols() << "\n";

    for (const auto& c : centroids) {
        out << c << "\n";
    }

}

void KMeansClusterer::fromStream(std::istream& in) {

    matchMetacommandOptional(in, "KMEANSCLUSTERER");

    in >> clusters;
    in >> max_iterations;

    size_t n_centroids;
    uint16_t wx, wy;
    in >> n_centroids;
    in >> wy;
    in >> wx;

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