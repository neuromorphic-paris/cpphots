/**
 * @file clustering.h
 * @brief Classes and functions for time surface clustering
 */
#ifndef CPPHOTS_CLUSTERING_H
#define CPPHOTS_CLUSTERING_H

#include "types.h"
#include "interfaces/clustering.h"


namespace cpphots {


/**
 * @brief HOTS basic clusterer
 * 
 * Clusters time surface according to the HOTS formulation (cosine rule).
 */
class HOTSClusterer : public interfaces::ClustererBase {

public:

    /**
     * @brief Construct a new HOTSClusterer
     * 
     * This constructor should never be used to create a new object,
     * it is provided only to create containers with Clusterer instances
     * or to read parameters from a file.
     */
    HOTSClusterer();

    /**
     * @brief Construct a new HOTSClusterer
     * 
     * The constructor will not initialize the prototypes.
     * 
     * @param clusters number of clusters
     */
    HOTSClusterer(uint16_t clusters);

    /**
     * @copydoc interfaces::Clusterer::cluster
     * 
     * If learning is enabled, this will update the prototypes.
     */
    uint16_t cluster(const TimeSurfaceType& surface) override;

    // inherited methods
    uint16_t getNumClusters() const override;

    void addPrototype(const TimeSurfaceType& proto) override;

    std::vector<TimeSurfaceType> getPrototypes() const override;

    void clearPrototypes() override;

    bool isInitialized() const override;

    /**
     * @copydoc interfaces::Clusterer::toggleLearning
     * 
     * If learning is enabled prototypes will be updatad online until
     * learning is disabled.
     */
    bool toggleLearning(bool enable = true) override;

    /**
     * @copydoc interfaces::Streamable::toStream
     * 
     * Insert parameters of the clusterer and prototypes on the stream.
     */
    void toStream(std::ostream& out) const override;

    /**
     * @copydoc interfaces::Streamable::fromStream
     * 
     * Reads parameters and prototypes from the stream.
     */
    void fromStream(std::istream& in) override;

private:
    std::vector<TimeSurfaceType> prototypes;
    std::vector<uint32_t> prototypes_activations;
    uint16_t clusters;
    bool learning = true;

};


/**
 * @brief Signature of clustering initialization algorithms
 */
using ClustererInitializerType = std::function<void(interfaces::Clusterer&, const std::vector<TimeSurfaceType>&)>;

/**
 * @brief Uniformly initialize the layer
 * 
 * Initializes the prototypes by simply choosing random time surfaces,
 * among those provided, with uniform probabilities.
 */
void ClustererUniformInitializer(interfaces::Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces);

/**
 * @brief k-means++ initialization
 * 
 * This function implements the initialization algorithm of k-means++ to choose the prototypes
 * among the time surfaces provided.
 */
void ClustererPlusPlusInitializer(interfaces::Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces);

/**
 * @brief AFK-MC2 clustering initialization
 * 
 * Return a function implementing AFK-MC2 clustering initialization as described in 
 * O. Bachem, M. Lucic, H. Hassani, and A. Krause. Fast and provably good seedings for k-means. In Proc. Advances in Neural Information Processing Systems, pages 55–63, 2016a
 * 
 * @param chain length of the Markov chain
 * @return the actual initialization function
 */
ClustererInitializerType ClustererAFKMC2Initializer(uint16_t chain);

/**
 * @brief Random clustering initialization
 * 
 * Return a function that initializes the prototypes of the layer with random time surfaces.
 * This may be useful for debugging purposes.
 * 
 * @param width width of the prototypes
 * @param height height of the prototypes
 * @return the actual initialization function
 */
ClustererInitializerType ClustererRandomInitializer(uint16_t width, uint16_t height);

}

#endif