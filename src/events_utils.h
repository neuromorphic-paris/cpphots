/**
 * @file events_utils.h
 * @brief Data structures and functions related to event management
 */

#ifndef EVENTS_UTILS_H
#define EVENTS_UTILS_H

#include <vector>
#include <cstdint>
#include <string>


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
 * @param merge_polarities whether or not to merge input polarities
 * @return the collection of events
 */
Events loadFromFile(const std::string& filename, bool merge_polarities = false);

}

#endif