/**
 * @file time_surface.h
 * @brief Time surface implementation
 */

#ifndef TIME_SURFACE_H
#define TIME_SURFACE_H

#include <ostream>
#include <istream>
#include <Eigen/Dense>
#include "events_utils.h"


namespace cpphots {

/**
 * @brief Alias type for a time surface
 */
using TimeSurfaceType = Eigen::ArrayXXf;  // using float instead of uint64_t for the initialization to -tau


/**
 * @brief Class that can compute time surfaces
 * 
 * This class keeps track of the time context for the current stream of events and can compute
 * the time surface for new ones.
 * 
 * The time surface has a linear activation as described in (Maro et al., 2020).
 * 
 * The time context and time surfaces are represented with Eigen Arrays.
 */
class TimeSurface {

public:

    /**
     * @brief Construct a new Time Surface object
     * 
     * This constructor should never be used explicitly,
     * it is provided only to create containers with Time Surface instances or to load Time Surfaces from files.
     */
    TimeSurface();

    /**
     * @brief Construct a new Time Surface object
     * 
     * The time context is initalized to -tau.
     * 
     * It is possible to use the whole time context in any dimension to compute the surfaces by setting Rx or Ry to 0.
     * However, this can break the assumption that the current event is centered in the time surface.
     * 
     * @param width width of the full time context
     * @param height height of the full time context
     * @param Rx horizontal radius of the window on which surfaces are computed (0 to use the full width)
     * @param Ry vertical radius of the window on which surfaces are computed (0 to use the full height)
     * @param tau time constant of the surface
     */
    TimeSurface(uint16_t width, uint16_t height, uint16_t Rx, uint16_t Ry, float tau);

    /**
     * @brief Update the time context with a new event
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     */
    void update(uint64_t t, uint16_t x, uint16_t y);
    
    /**
     * @brief Update the time context with a new event
     * 
     * @param ev the new event
     */
    inline void update(const event& ev) {
        update(ev.t, ev.x, ev.y);
    }

    /**
     * @brief Compute the time surface for an event
     * 
     * This function does not update the time context.
     * 
     * A time surface is considered valid if there is a certain number of relevant events,
     * determined using an heuristic extended from the one described in the paper.
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    std::pair<TimeSurfaceType, bool> compute(uint64_t t, uint16_t x, uint16_t y) const;

    /**
     * @brief Compute the time surface for an event
     * 
     * This function does not update the time context.
     * 
     * A time surface is considered valid if there is a certain number of relevant events,
     * determined using an heuristic extended from the on described in the paper.
     * 
     * @param ev the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    inline std::pair<TimeSurfaceType, bool> compute(const event& ev) const {
        return compute(ev.t, ev.x, ev.y);
    }

    /**
     * @brief Update the time context and compute the new surface
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    inline std::pair<TimeSurfaceType, bool> updateAndCompute(uint64_t t, uint16_t x, uint16_t y) {
        update(t, x, y);
        return compute(t, x, y);
    }

    /**
     * @brief Update the time context and compute the new surface
     * 
     * @param ev the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    inline std::pair<TimeSurfaceType, bool> updateAndCompute(const event& ev) {
        update(ev.t, ev.x, ev.y);
        return compute(ev.t, ev.x, ev.y);
    }

    /**
     * @brief Get the temporal context
     * 
     * This function returns the full temporal context on which surfaces are computed.
     * 
     * @return the temporal context
     */
    inline const TimeSurfaceType& getFullContext() const {
        return context;
    }

    /**
     * @brief Sample and decay all temporal context
     * 
     * This functions applies the decay to the full temporal context and returns it.
     * 
     * @param t sample time
     * @return Decayed temporal context
     */
    TimeSurfaceType sampleFullContext(uint64_t t) const;

    /**
     * @brief Reset the time context
     * 
     * The time context is reset to -tau
     */
    void reset();

    /**
     * @brief Get the horizontal size of the window
     * 
     * @return the horizontal size of the window
     */
    inline uint16_t getWx() const {
        return Wx;
    }

    /**
     * @brief Get the vertical size of the window
     * 
     * @return the vertical size of the window
     */
    inline uint16_t getWy() const {
        return Wy;
    }

    /**
     * @brief Stream insertion operator for Time Surfaces
     * 
     * Puts all parameters in sequence on the stream, but leaves out the time context.
     * 
     * @param out output stream
     * @param ts TimeSurface to insert
     * @return output stream
     */
    friend std::ostream& operator<<(std::ostream& out, const TimeSurface& ts);

    /**
     * @brief Stream extraction operator for Time Surfaces
     * 
     * Reads all parameters from the stream. Previous parameters are overwritten.
     * 
     * @param in input stream
     * @param ts TimeSurface where to extract into
     * @return input stream
     */
    friend std::istream& operator>>(std::istream& in, TimeSurface& ts);

private:

    TimeSurfaceType context;
    uint16_t width, height;
    uint16_t Rx, Ry;
    uint16_t Wx, Wy;
    float tau;
    uint16_t min_events;

};

}

#endif