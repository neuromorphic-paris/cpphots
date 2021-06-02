/**
 * @file load.h
 * @brief Functions to load components and networks from streams
 */
#ifndef CPPHOTS_LOAD_H
#define CPPHOTS_LOAD_H

#include <istream>

#include "time_surface.h"
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
 * @brief Construct a layer from an input stream
 * 
 * This function can be used to load a layer from a stream,
 * without knowing a priori the components of the layer.
 * 
 * @param in input stream
 * @return pointer to the new layer
 */
// LayerPtr loadLayerFromStream(std::istream& in);

}

#endif