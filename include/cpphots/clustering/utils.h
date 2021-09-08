/**
 * @file clustering_utils.h
 * @brief Utility functions for time surface clustering
 */
#ifndef CPPHOTS_CLUSTERING_UTILS_H
#define CPPHOTS_CLUSTERING_UTILS_H

#include "../types.h"
#include "../interfaces/clustering.h"


namespace cpphots {

/**
 * @brief Base class that can manage the histogram
 */
class ClustererBase : public virtual interfaces::Clusterer {

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


/**
 * @brief Signature of clustering seeding algorithms
 */
using ClustererSeedingType = std::function<void(interfaces::Clusterer&, const std::vector<TimeSurfaceType>&)>;

/**
 * @brief Uniformly seed the layer
 * 
 * Seed the centroids by simply choosing random time surfaces,
 * among those provided, with uniform probabilities.
 */
void ClustererUniformSeeding(interfaces::Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces);

/**
 * @brief k-means++ seeding
 * 
 * This function implements the seeding algorithm of k-means++ to choose the centroids
 * among the time surfaces provided.
 */
void ClustererPlusPlusSeeding(interfaces::Clusterer& clusterer, const std::vector<TimeSurfaceType>& time_surfaces);

/**
 * @brief AFK-MC2 clustering seeding
 * 
 * Return a function implementing AFK-MC2 clustering seeding as described in 
 * O. Bachem, M. Lucic, H. Hassani, and A. Krause. Fast and provably good seedings for k-means.
 * In Proc. Advances in Neural Information Processing Systems, pages 55–63, 2016a
 * 
 * @param chain length of the Markov chain
 * @return the actual seeding function
 */
ClustererSeedingType ClustererAFKMC2Seeding(uint16_t chain);

/**
 * @brief Random clustering seeding
 * 
 * Return a function that seeds the centroids of the layer with random time surfaces.
 * This may be useful for debugging purposes.
 * 
 * @param width width of the centroids
 * @param height height of the centroids
 * @return the actual seeding function
 */
ClustererSeedingType ClustererRandomSeeding(uint16_t width, uint16_t height);

}

#endif