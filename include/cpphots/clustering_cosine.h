/**
 * @file clustering_cosine.h
 * @brief Clustering based on cosine similarity
 */
#ifndef CPPHOTS_CLUSTERING_COSINE_H
#define CPPHOTS_CLUSTERING_COSINE_H

#include "types.h"
#include "interfaces/clustering.h"


namespace cpphots {

/**
 * @brief HOTS basic clusterer
 * 
 * Clusters time surface according to the HOTS formulation (cosine rule).
 */
class CosineClusterer : public interfaces::ClustererBase {

public:

    /**
     * @brief Construct a new CosineClusterer
     * 
     * This constructor should never be used to create a new object,
     * it is provided only to create containers with Clusterer instances
     * or to read parameters from a file.
     */
    CosineClusterer();

    /**
     * @brief Construct a new CosineClusterer
     * 
     * The constructor will not initialize the prototypes.
     * 
     * @param clusters number of clusters
     */
    CosineClusterer(uint16_t clusters);

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

}

#endif