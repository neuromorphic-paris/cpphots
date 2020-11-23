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
#include <ostream>
#include <limits>


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
 * @brief An invalid event
 */
inline const event invalid_event{std::numeric_limits<uint64_t>::max(),
                                 std::numeric_limits<uint16_t>::max(),
                                 std::numeric_limits<uint16_t>::max(),
                                 std::numeric_limits<uint16_t>::max()};

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

/**
 * @brief Stream insertion operator for events
 * 
 * Insert the event as "(t, x, y, p)".
 * 
 * @param out output stream
 * @param event an event
 * @return output stream
 */
std::ostream& operator<<(std::ostream& out, const event& event);


/**
 * @brief Equality operator for events
 * 
 * @param ev1 first event
 * @param ev2 second event
 * @return true if the two events are the same
 * @return false otherwise
 */
bool operator==(const event& ev1, const event& ev2);


/**
 * @brief Inequality operator for events
 * 
 * @param ev1 first event
 * @param ev2 second event
 * @return true if the two events are not the same
 * @return false otherwise
 */
inline bool operator!=(const event& ev1, const event& ev2) {
    return !(ev1 == ev2);
}

}

#endif