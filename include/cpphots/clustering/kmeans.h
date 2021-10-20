/**
 * @file clustering/kmeans.h
 * @brief K-means clustering
 */
#ifndef CPPHOTS_CLUSTERING_KMEANS_H
#define CPPHOTS_CLUSTERING_KMEANS_H

#include "../types.h"
#include "../interfaces/clustering.h"
#include "utils.h"

namespace cpphots {

class KMeansClusterer : public interfaces::Clonable<KMeansClusterer, interfaces::Clusterer>, public ClustererHistogramMixin, public ClustererOfflineMixin {

public:

    KMeansClusterer();

    KMeansClusterer(uint16_t clusters, uint16_t max_iterations = 1000);

    uint16_t cluster(const TimeSurfaceType& surface) override;

    uint16_t getNumClusters() const override;

    void addCentroid(const TimeSurfaceType& centroid) override;

    const std::vector<TimeSurfaceType>& getCentroids() const override;

    void clearCentroids() override;

    bool hasCentroids() const override;

    void train(const std::vector<TimeSurfaceType>& tss) override;

    /**
     * @copydoc interfaces::Streamable::toStream
     * 
     * Insert parameters of the clusterer and centroids on the stream.
     */
    void toStream(std::ostream& out) const override;

    /**
     * @copydoc interfaces::Streamable::fromStream
     * 
     * Reads parameters and centroids from the stream.
     */
    void fromStream(std::istream& in) override;

private:
    std::vector<TimeSurfaceType> centroids;
    uint16_t clusters, max_iterations;

};

}

#endif