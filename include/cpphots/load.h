/**
 * @file load.h
 * @brief Functions to load components and networks from streams
 */
#ifndef CPPHOTS_LOAD_H
#define CPPHOTS_LOAD_H

#include <istream>

#include "interfaces/time_surface.h"
#include "interfaces/clustering.h"
#include "interfaces/layer_modifiers.h"
#include "layer.h"
#include "network.h"


namespace cpphots {

/**
 * @brief Construct a time surface from an input stream
 * 
 * This function can be used to load a time surface from a stream,
 * without knowing a priori the type of the time surface.
 * 
 * @param in input stream
 * @return pointer to the new time surface 
 */
TimeSurfacePtr loadTSFromStream(std::istream& in);

/**
 * @brief Construct a time surface pool from an input stream
 * 
 * @param in input stream
 * @return pointer to the new time surface pool
 */
interfaces::TimeSurfacePoolCalculator* loadTSPoolFromStream(std::istream& in);

/**
 * @brief Construct a clusterer from an input stream
 * 
 * @param in input stream
 * @return pointer to the new clusterer
 */
interfaces::Clusterer* loadClustererFromStream(std::istream& in);

/**
 * @brief Construct an event remapper from an input stream
 * 
 * @param in input stream
 * @return pointer to the new event remapper
 */
interfaces::EventRemapper* loadRemapperFromStream(std::istream& in);

/**
 * @brief Construct a super cell modifier from an input stream
 * 
 * @param in input stream
 * @return pointer to the new super cell modifier
 */
interfaces::SuperCell* loadSuperCellFromStream(std::istream& in);

}

#endif