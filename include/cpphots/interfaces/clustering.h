/**
 * @file interfaces/clustering.h
 * @brief Interfaces for clustering
 */
#ifndef CPPHOTS_INTERFACES_CLUSTERING_H
#define CPPHOTS_INTERFACES_CLUSTERING_H


#include <vector>

#include "../types.h"
#include "streamable.h"
#include "clonable.h"


namespace cpphots {

namespace interfaces {

/**
 * @brief Interface for time surface clustering based on centroids
 */
class Clusterer : public virtual Streamable, public ClonableBase<Clusterer> {

public:

    /**
     * @brief Destroy the Clusterer object
     */
    ~Clusterer() {}

    /**
     * @brief Performs clustering
     * 
     * @param surface the timesurface to cluster
     * @return id of the cluster
     */
    virtual uint16_t cluster(const TimeSurfaceType& surface) = 0;

    /**
     * @brief Get the number of clusters
     * 
     * @return the number of clusters
     */
    virtual uint16_t getNumClusters() const = 0;

    /**
     * @brief Add a new centroid to the clusterer
     * 
     * This function should not be used manually, seeding should be done via a ClustererSeeding function.
     * 
     * If the Clusterer already has enough centroids, an exception is raised.
     * 
     * @param centroid the centroid to add
     */
    virtual void addCentroid(const TimeSurfaceType& centroid) = 0;

    /**
     * @brief Get the list of centroids
     * 
     * @return the list of centroids
     */
    virtual std::vector<TimeSurfaceType> getCentroids() const = 0;

    /**
     * @brief Remove all centroids
     */
    virtual void clearCentroids() = 0;

    /**
     * @brief Check if clusterer has enough centroids
     * 
     * A clusterer has centroids if there is a number of centroids equal to the number of clusters.
     * 
     * @return true if clusterer has centroids
     * @return false otherwise
     */
    virtual bool hasCentroids() const = 0;

    /**
     * @brief Enable or disable learning
     * 
     * This affects whether the centroids are updated when Clusterer::cluster is called or not.
     * 
     * During the learning phase, output of Clusterer::cluster may be undefined, depending
     * on the implementaton.
     * 
     * @param enable true if learning should be active, false otherwise
     * @return the previous learning state
     */
    virtual bool toggleLearning(bool enable = true) = 0;

    /**
     * @brief Get the histogram of centroids activations
     * 
     * @return the histogram of activations
     */
    virtual std::vector<uint32_t> getHistogram() const = 0;

    /**
     * @brief Reset the histogram of activations
     */
    virtual void reset() = 0;

};


/**
 * @brief Base class that can manage the histogram
 */
class ClustererBase : public virtual Clusterer {

public:

    std::vector<uint32_t> getHistogram() const override;

    void reset() override;

protected:
    /**
     * @brief Updated the histogram of activations
     * 
     * This function should be called by subclasses in their cluster implementation.
     * 
     * @param k the latest cluster id emitted
     */
    void updateHistogram(uint16_t k);

private:

    std::vector<uint32_t> hist;

};

}


}


#endif