/**
 * @file clustering/utils.h
 * @brief Utility functions and class mixins for time surface clustering
 */
#ifndef CPPHOTS_CLUSTERING_UTILS_H
#define CPPHOTS_CLUSTERING_UTILS_H

#include "../types.h"
#include "../interfaces/clustering.h"


namespace cpphots {

/**
 * @brief Mixin for clusterer that can manage the histogram
 */
class ClustererHistogramMixin : public virtual interfaces::Clusterer {

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
 * @brief Mixin for online clusterers
 */
class ClustererOnlineMixin : public virtual interfaces::Clusterer {

public:

    bool isOnline() const final;

    /**
     * @copydoc interfaces::Clusterer::train
     * 
     * Enable learning, call cluster on every element and disable learning.
     */
    void train(const std::vector<TimeSurfaceType>& tss) override;

};


/**
 * @brief Mixin for offline clusterers
 * 
 * Classes using this mixin must call cluster in order to have it working correctly.
 */
class ClustererOfflineMixin : public virtual interfaces::Clusterer {

public:

    /**
     * @brief Performs clustering
     * 
     * If learning is enabled store the time surface.
     * If learning is disabled do nothing.
     * 
     * @param surface the timesurface to cluster
     * @return 0
     */
    uint16_t cluster(const TimeSurfaceType& surface) override;

    bool isOnline() const final;

    /**
     * @copydoc interfaces::Clusterer::toggleLearning
     * 
     * If learning is enabled start to store time surfaces.
     * If learning is disabled call train with stored time surfaces.
     */
    bool toggleLearning(bool enable = true) override;

protected:
    /**
     * @brief Check if learning is active
     * 
     * @return true if learning is active
     * @return false otherwise
     */
    bool isLearning() const;

private:
    std::vector<TimeSurfaceType> learning_tss;
    bool learning = false;

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