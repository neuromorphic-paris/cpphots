#ifndef CLUSTERING_H
#define CLUSTERING_H

#include "time_surface.h"

namespace cpphots {

class ClustererBase {

public:
    virtual uint16_t cluster(const TimeSurfaceType& surface) = 0;

};


class Clusterer : public ClustererBase {

public:

    /**
     * @brief Construct a new Clusterer
     * 
     * This constructor should never be used to create a new object,
     * it is provided only to create containers with Clustere instances
     * or to read parameters from a file.
     */
    Clusterer();

    /**
     * @brief Construct a new Clusterer
     * 
     * The constructor will not initialize the prototypes.
     * 
     * @param clusters number of clusters
     */
    Clusterer(uint16_t clusters);

    /**
     * @brief Performs clustering
     * 
     * @param surface the timesurface to cluster
     * @return id of the cluster
     */
    uint16_t cluster(const TimeSurfaceType& surface) override;

    /**
     * @brief Get the number of clusters
     * 
     * @return the number of clusters
     */
    uint16_t getNumClusters() const;

    /**
     * @brief Get the list of prototypes
     * 
     * @return the list of prototypes
     */
    std::vector<TimeSurfaceType> getPrototypes() const;

    /**
     * @brief Enable or disable learning
     * 
     * This affects whether the prototypes are updated when cluster is called or not.
     * 
     * @param enable true if learning should be active, false otherwise
     * @return the previous learning state
     */
    bool toggleLearning(bool enable = true);

    /**
     * @brief Check prototype initialization
     * 
     * Prototypes are initialized if there is a number of prototypes equal to the number of clusters.
     * 
     * @return true if prototypes are initialized
     * @return false otherwise
     */
    inline bool isInitialized() const {
        return (prototypes.size() == clusters) && (prototypes_activations.size() == clusters);
    }

    /**
     * @brief Removes all prototypes
     */
    void clearPrototypes();

    /**
     * @brief Add a new prototype to the layer
     * 
     * This function should not be used manually, initialization should be done via a LayerInitializer.
     * 
     * If the Clusterer is already initialized, an exception is raised.
     * 
     * @param proto the prototype to add
     */
    void addPrototype(const TimeSurfaceType& proto);

    /**
     * @brief Get the histogram of prototypes activations
     * 
     * @return the histogram of activations
     */
    std::vector<uint32_t> getHistogram() const;

    /**
     * @brief Reset the histogram of activations
     */
    void reset();

    /**
     * @brief Stream insertion operator for Clusterer
     * 
     * Insert parameters of the clusterer and prototypes on the stream.
     * 
     * @param out output stream
     * @param clusterer clusterer to insert
     * @return output stream
     */
    friend std::ostream& operator<<(std::ostream& out, const Clusterer& clusterer);

    /**
     * @brief Stream extraction operator for Clusterer
     * 
     * Reads parameters and prototypes for the Clusterer. Previous parameters are overwritten.
     * 
     * @param in input stream
     * @param clusterer Clusterer where to extract into
     * @return input stream
     */
    friend std::istream& operator>>(std::istream& in, Clusterer& clusterer);

private:
    std::vector<TimeSurfaceType> prototypes;
    std::vector<uint32_t> prototypes_activations;
    uint16_t clusters;
    std::vector<uint32_t> hist;
    bool learning = true;

};

}

#endif