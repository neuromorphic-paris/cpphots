/**
 * @file interfaces/clustering.h
 * @brief Interfaces for clustering
 */
#ifndef CPPHOTS_INTERFACES_CLUSTERING_H
#define CPPHOTS_INTERFACES_CLUSTERING_H


#include <vector>

#include "../types.h"
#include "streamable.h"


namespace cpphots {

namespace interfaces {

/**
 * @brief Interface for time surface clustering
 */
class Clusterer : public virtual Streamable {

public:
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
     * @brief Add a new prototype to the layer
     * 
     * This function should not be used manually, initialization should be done via a ClustererInitialize function.
     * 
     * If the HOTSClusterer is already initialized, an exception is raised.
     * 
     * @param proto the prototype to add
     */
    virtual void addPrototype(const TimeSurfaceType& proto) = 0;

    /**
     * @brief Get the list of prototypes
     * 
     * @return the list of prototypes
     */
    virtual std::vector<TimeSurfaceType> getPrototypes() const = 0;

    /**
     * @brief Removes all prototypes
     */
    virtual void clearPrototypes() = 0;

    /**
     * @brief Check prototype initialization
     * 
     * Prototypes are initialized if there is a number of prototypes equal to the number of clusters.
     * 
     * @return true if prototypes are initialized
     * @return false otherwise
     */
    virtual bool isInitialized() const = 0;

    /**
     * @brief Enable or disable learning
     * 
     * This affects whether the prototypes are updated when Clusterer::cluster is called or not.
     * 
     * During the learning phase, output of Clusterer::cluster may be undefined, depending
     * on the implementaton.
     * 
     * @param enable true if learning should be active, false otherwise
     * @return the previous learning state
     */
    virtual bool toggleLearning(bool enable = true) = 0;

    /**
     * @brief Get the histogram of prototypes activations
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

    std::vector<uint32_t> getHistogram() const;

    void reset();

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