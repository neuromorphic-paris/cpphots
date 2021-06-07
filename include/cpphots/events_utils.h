/**
 * @file events_utils.h
 * @brief Functions related to event management
 */

#ifndef CPPHOTS_EVENTS_UTILS_H
#define CPPHOTS_EVENTS_UTILS_H

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <ostream>
#include <limits>

#include "types.h"


namespace cpphots {

/**
 * @brief Load events from a file
 * 
 * File is espected to be EventStream, as it will be parsed with Sepia.
 * 
 * @param filename path to the file
 * @param change_polarities a {bool: uint16_t} dictionary that specifies how to handle the conversion between boolean and uint polarities (can be used to merge polarities)
 * @return the collection of events
 */
Events loadFromFile(const std::string& filename, const std::unordered_map<bool, uint16_t>& change_polarities = {{false, 0}, {true, 1}});


}

#endif