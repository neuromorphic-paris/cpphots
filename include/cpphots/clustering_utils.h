/**
 * @file clustering_utils.h
 * @brief Utility functions for time surface clustering
 */
#ifndef CPPHOTS_CLUSTERING_UTILS_H
#define CPPHOTS_CLUSTERING_UTILS_H

#include "types.h"
#include "interfaces/clustering.h"


namespace cpphots {

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