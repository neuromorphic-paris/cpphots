/**
 * @file clustering/gmm.h
 * @brief Clustering with variational gaussian mixture models
 */
#ifndef CPPHOTS_CLUSTERING_GMM_H
#define CPPHOTS_CLUSTERING_GMM_H

#include <memory>

#include <blaze/Math.h>

#include "../types.h"
#include "../interfaces/clustering.h"
#include "utils.h"


// forward declarations from peregrine
template <typename T>
class Gmm_core;

template <typename T>
struct dataset;


namespace cpphots {

/**
 * @brief Blaze vector type
 */
using BlazeVector = blaze::DynamicVector<TimeSurfaceScalarType, blaze::rowVector>;

/**
 * @brief Blaze matrix type
 */
using BlazeMatrix = blaze::DynamicMatrix<TimeSurfaceScalarType, blaze::rowMajor>;

/**
 * @brief Clustering using variational gaussian mixture models
 * 
 * See https://github.com/OOub/peregrine for more details.
 */
class GMMClusterer : public interfaces::Clonable<GMMClusterer, interfaces::Clusterer>, public ClustererHistogramMixin, public ClustererOfflineMixin {

public:

    /**
     * @brief GMM algorithm options
     */
    enum GMMType {
        S_GMM,
        U_S_GMM
    };

    /**
     * @brief Construct a new GMMClusterer
     * 
     * This constructor should never be used to create a new object,
     * it is provided only to create containers with Clusterer instances
     * or to read parameters from a file.
     */
    GMMClusterer();

    /**
     * @brief Construct a new GMMClusterer
     * 
     * @param type algorithm type
     * @param clusters number of clusters
     * @param truncated_clusters number of truncated clusters
     * @param clusters_considered number of neighboring clusters considered
     * @param eps convergence criterion, precision if < 1, number of epochs if > 1
     */
    GMMClusterer(GMMType type, uint16_t clusters, uint16_t truncated_clusters, uint16_t clusters_considered, TimeSurfaceScalarType eps);

    /**
     * @copydoc interfaces::Clusterer::cluster
     * 
     * If learning is enabled the time surfaces will be stored in memory
     * and output will always be 0.
     */
    uint16_t cluster(const TimeSurfaceType& surface) override;

    uint16_t getNumClusters() const override;

    void addCentroid(const TimeSurfaceType& centroid) override;

    std::vector<TimeSurfaceType> getCentroids() const override;

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
    GMMType type;
    std::shared_ptr<Gmm_core<TimeSurfaceScalarType>> algo;
    uint16_t clusters, truncated_clusters, clusters_considered;
    BlazeMatrix mean;
    std::shared_ptr<dataset<TimeSurfaceScalarType>> set;
    size_t last_centroid = 0;
    bool learning = true;
    TimeSurfaceScalarType eps;
    std::pair<uint16_t, uint16_t> ts_shape;

    void fit();

    uint16_t predict(const BlazeVector& vec, int top_k = 1);

};

}

#endif