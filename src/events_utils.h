/**
 * @file events_utils.h
 * @brief Data structures and functions related to event management
 */

#ifndef EVENTS_UTILS_H
#define EVENTS_UTILS_H

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>


namespace cpphots {

/**
 * @brief Structure representing an event
 * 
 * This structure supports events with an arbitrary number of polarities,
 * as this is needed to trasmit events among different HOTS layers.
 */
struct __attribute__((__packed__)) event {

        /**
         * @brief timestamp of the event
         */
        uint64_t t;

        /**
         * @brief horizontal coordinate of the event
         */
        uint16_t x;

        /**
         * @brief vertical coordinate of the event
         */
        uint16_t y;

        /**
         * @brief polarity of the event
         */
        uint16_t p;

};

/**
 * @brief A stream of events
 * 
 * This is a typedef of std::vector<event>.
 */
using Events = std::vector<event>;

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