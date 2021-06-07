/**
 * @file types.h
 * @brief Common types used by the library
 */
#ifndef CPPHOTS_TYPES_H
#define CPPHOTS_TYPES_H


#include <Eigen/Dense>


namespace cpphots {

/**
 * @brief Alias type for a time surface
 */
#ifdef CPPHOTS_DOUBLE_PRECISION
using TimeSurfaceType = Eigen::ArrayXXd;  // using floating point instead of uint64_t for the initialization to -tau
#else
using TimeSurfaceType = Eigen::ArrayXXf;  // using floating point instead of uint64_t for the initialization to -tau
#endif

/**
 * @brief Alias type for time surface floating point type
 */
using TimeSurfaceScalarType = TimeSurfaceType::Scalar;


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

/**
 * @brief A stream of events
 * 
 * This is a typedef of std::vector<event>.
 */
using Events = std::vector<event>;


}

#endif