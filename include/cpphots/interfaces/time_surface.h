/**
 * @file interfaces/time_surface.h
 * @brief Interfaces for time surface computation
 */
#ifndef CPPHOTS_INTERFACES_TIME_SURFACE_H
#define CPPHOTS_INTERFACES_TIME_SURFACE_H

#include <ostream>
#include <istream>
#include <memory>

#include "../types.h"
#include "streamable.h"
#include "clonable.h"


namespace cpphots {

namespace interfaces {

/**
 * @brief Interface for that can compute time surfaces
 */
class TimeSurfaceCalculator : public virtual interfaces::Streamable, public ClonableBase<TimeSurfaceCalculator> {

public:

    /**
     * @brief Destroy the TimeSurfaceCalculator
     */
    virtual ~TimeSurfaceCalculator() {}

    /**
     * @brief Update the time context with a new event
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     */
    virtual void update(uint64_t t, uint16_t x, uint16_t y) = 0;

    /**
     * @brief Update the time context with a new event
     * 
     * @param ev the new event
     */
    virtual void update(const event& ev) = 0;

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
    virtual std::pair<TimeSurfaceType, bool> compute(uint64_t t, uint16_t x, uint16_t y) const = 0;

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
    virtual std::pair<TimeSurfaceType, bool> compute(const event& ev) const = 0;

    /**
     * @brief Update the time context and compute the new surface
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    virtual std::pair<TimeSurfaceType, bool> updateAndCompute(uint64_t t, uint16_t x, uint16_t y) = 0;

    /**
     * @brief Update the time context and compute the new surface
     * 
     * @param ev the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    virtual std::pair<TimeSurfaceType, bool> updateAndCompute(const event& ev) = 0;

    /**
     * @brief Get the temporal context
     * 
     * This function returns the full temporal context (including padding)
     * on which surfaces are computed.
     * 
     * You may want to use getContext() in normal circumnstances.
     * 
     * @return the temporal context
     */
    virtual const TimeSurfaceType& getFullContext() const = 0;

    /**
     * @brief Get the temporal context
     * 
     * This function returns the whole temporal context on which surfaces are computed.
     * 
     * @return the temporal context
     */
    virtual TimeSurfaceType getContext() const = 0;

    /**
     * @brief Sample and decay all temporal context
     * 
     * This functions applies the decay to the whole temporal context and returns it.
     * 
     * @param t sample time
     * @return decayed temporal context
     */
    virtual TimeSurfaceType sampleContext(uint64_t t) const = 0;

    /**
     * @brief Reset the time context
     * 
     * The time context is reset to -tau
     */
    virtual void reset() = 0;

    /**
     * @brief Returns the size of the context
     * 
     * @return {width, height}
     */
    virtual std::pair<uint16_t, uint16_t> getSize() const = 0;

    /**
     * @brief Get the horizontal size of the window
     * 
     * @return the horizontal size of the window
     */
    virtual uint16_t getWx() const = 0;

    /**
     * @brief Get the vertical size of the window
     * 
     * @return the vertical size of the window
     */
    virtual uint16_t getWy() const = 0;

};

}

/**
 * @brief Alias for a pointer to a generic time surface
 */
using TimeSurfacePtr = interfaces::TimeSurfaceCalculator*;


namespace interfaces {

/**
 * @brief Interface for a pool of time surface calculators
 * 
 * This class holds a pool of time surfaces so that it can dispatch
 * events with different polarities to the appropriate time surface.
 * 
 */
class TimeSurfacePoolCalculator : public virtual interfaces::Streamable, public ClonableBase<TimeSurfacePoolCalculator> {

public:

    /**
     * @brief Destroy the TimeSurfacePoolCalculator
     * 
     */
    virtual ~TimeSurfacePoolCalculator() {}

    /**
     * @brief Update the time context with a new event
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @param p polarity of the event
     */
    virtual void update(uint64_t t, uint16_t x, uint16_t y, uint16_t p) = 0;

    /**
     * @brief Update the time context with a new event
     * 
     * @param ev the new event
     */
    virtual void update(const event& ev) = 0;

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
     * @param p polarity of the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    virtual std::pair<TimeSurfaceType, bool> compute(uint64_t t, uint16_t x, uint16_t y, uint16_t p) const = 0;

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
    virtual std::pair<TimeSurfaceType, bool> compute(const event& ev) const = 0;

    /**
     * @brief Update the time context and compute the new surface
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @param p polarity of the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    virtual std::pair<TimeSurfaceType, bool> updateAndCompute(uint64_t t, uint16_t x, uint16_t y, uint16_t p) = 0;

    /**
     * @brief Update the time context and compute the new surface
     * 
     * @param ev the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    virtual std::pair<TimeSurfaceType, bool> updateAndCompute(const event& ev) = 0;

    /**
     * @brief Returns the size of the context
     * 
     * @return {width, height}
     */
    virtual std::pair<uint16_t, uint16_t> getSize() const = 0;

    /**
     * @brief Reset the time surfaces
     * 
     * This method resets the time surfaces.
     * It should be called before every stream of events.
     */
    virtual void reset() = 0;

    /**
     * @brief Access a time surface with boundaries check
     * 
     * This method will throw an invalid_argument exception if the index of the
     * surfaces exceeds the number of actual time surfaces.
     * 
     * @param idx index of the time surface
     * @return reference to the time surface
     */
    virtual TimeSurfacePtr& getSurface(size_t idx) = 0;

    /**
     * @brief Access a time surface with boundaries check
     * 
     * This method will throw an invalid_argument exception if the index of the
     * surfaces exceeds the number of actual time surfaces.
     * 
     * @param idx index of the time surface
     * @return reference to the time surface
     */
    virtual const TimeSurfacePtr& getSurface(size_t idx) const = 0;

    /**
     * @brief Sample and decay all temporal contexts from the pool
     * 
     * This functions applies the decay to the all temporal contexts and returns them.
     * 
     * @param t sample time
     * @return vector of decayed temporal contexts
     */
    virtual std::vector<TimeSurfaceType> sampleContexts(uint64_t t) const = 0;

    /**
     * @brief Get the number of surfaces in the pool
     * 
     * @return number of surfaces
     */
    virtual size_t getNumSurfaces() const = 0;

};

}

}

#endif