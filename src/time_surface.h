/**
 * @file time_surface.h
 * @brief Time surface implementation
 */

#ifndef TIME_SURFACE_H
#define TIME_SURFACE_H

#include <eigen3/Eigen/Dense>
#include "events_utils.h"


namespace cpphots {

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
     * it is provided only to create containers with Time Surface instances.
     */
    TimeSurface();

    /**
     * @brief Construct a new Time Surface object
     * 
     * The time context is initalized to -tau.
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
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @param t time of the event
     */
    void update(uint16_t x, uint16_t y, uint64_t t);
    
    /**
     * @brief Update the time context with a new event
     * 
     * @param ev the new event
     */
    inline void update(const event& ev) {
        update(ev.x, ev.y, ev.t);
    }

    /**
     * @brief Compute the time surface for an event
     * 
     * This function does not update the time context.
     * 
     * A time surface is considered valid if there is a certain number of relevant events,
     * determined using an heuristic extended from the on described in the paper.
     * 
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @param t time of the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    std::pair<Eigen::ArrayXXf, bool> compute(uint16_t x, uint16_t y, uint64_t t) const;

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
    inline std::pair<Eigen::ArrayXXf, bool> compute(const event& ev) {
        return compute(ev.x, ev.y, ev.t);
    }

    /**
     * @brief Update the time context and compute the new surface
     * 
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @param t time of the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    inline std::pair<Eigen::ArrayXXf, bool> updateAndCompute(uint16_t x, uint16_t y, uint64_t t) {
        update(x, y, t);
        return compute(x, y, t);
    }

    /**
     * @brief Update the time context and compute the new surface
     * 
     * @param ev the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    inline std::pair<Eigen::ArrayXXf, bool> updateAndCompute(const event& ev) {
        update(ev.x, ev.y, ev.t);
        return compute(ev.x, ev.y, ev.t);
    }

    /**
     * @brief Get the temporal context
     * 
     * This function returns the full temporal context on which surfaces are computed
     * 
     * @return the temporal context
     */
    inline const Eigen::ArrayXXf& getFullContext() const {
        return context;
    }

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

private:

    Eigen::ArrayXXf context;  // using float instead of uint64_t for the initialization to -tau
    uint16_t width, height;
    uint16_t Rx, Ry;
    uint16_t Wx, Wy;
    float tau;
    uint16_t min_events;

};

}

#endif