/**
 * @file time_surface.h
 * @brief Time surface implementation
 */
#ifndef CPPHOTS_TIME_SURFACE_H
#define CPPHOTS_TIME_SURFACE_H

#include <ostream>
#include <istream>
#include <memory>

#include <Eigen/Dense>

#include "events_utils.h"
#include "interfaces.h"


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
 * @brief Base class for classes that can compute time surfaces
 * 
 * This class keeps track of the time context for the current stream of events,
 * but it's up to the suclasses to compute the time surfaces.
 */
class TimeSurfaceBase : public virtual Streamable {

public:

    /**
     * @brief Construct a new Time Surface object
     * 
     * This constructor is provided only to create containers
     * with time surfaces instances or to load a time surface from file.
     * It should not be used to create an usable time surface object.
     */
    TimeSurfaceBase();

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
    TimeSurfaceBase(uint16_t width, uint16_t height, uint16_t Rx, uint16_t Ry, TimeSurfaceScalarType tau);

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
    void update(const event& ev) {
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
    std::pair<TimeSurfaceType, bool> updateAndCompute(uint64_t t, uint16_t x, uint16_t y) {
        update(t, x, y);
        return compute(t, x, y);
    }

    /**
     * @brief Update the time context and compute the new surface
     * 
     * @param ev the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    std::pair<TimeSurfaceType, bool> updateAndCompute(const event& ev) {
        update(ev.t, ev.x, ev.y);
        return compute(ev.t, ev.x, ev.y);
    }

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
    const TimeSurfaceType& getFullContext() const {
        return context;
    }

    /**
     * @brief Get the temporal context
     * 
     * This function returns the whole temporal context on which surfaces are computed.
     * 
     * @return the temporal context
     */
    TimeSurfaceType getContext() const;

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
    void reset();

    /**
     * @brief Returns the size of the context
     * 
     * @return {width, height}
     */
    std::pair<uint16_t, uint16_t> getSize() const;

    /**
     * @brief Get the horizontal size of the window
     * 
     * @return the horizontal size of the window
     */
    uint16_t getWx() const {
        return Wx;
    }

    /**
     * @brief Get the vertical size of the window
     * 
     * @return the vertical size of the window
     */
    uint16_t getWy() const {
        return Wy;
    }

    /**
     * @copydoc Streamable::toStream
     * 
     * Does not save the current time context.
     */
    void toStream(std::ostream& out) const override;

    void fromStream(std::istream& in) override;

protected:

    /**
     * @brief Time context
     */
    TimeSurfaceType context;

    /**
     * @brief Width of the context
     */
    uint16_t width;

    /**
     * @brief Height of the context
     */
    uint16_t height;

    /**
     * @brief Horizontal radius of the window
     */
    uint16_t Rx;

    /**
     * @brief Vertical radius of the window
     */
    uint16_t Ry;

    /**
     * @brief Horizontal size of the window
     */
    uint16_t Wx;

    /**
     * @brief Vertical size of the window
     */
    uint16_t Wy;

    /**
     * @brief Time constant
     */
    TimeSurfaceScalarType tau;

    /**
     * @brief Minimum number of events for a valid surface
     */
    uint16_t min_events;

};

/**
 * @brief Alias for a pointer to a generic time surface
 */
using TimeSurfacePtr = std::shared_ptr<TimeSurfaceBase>;


/**
 * @brief Class that can compute linear time surfaces
 * 
 * This class keeps track of the time context for the current stream of events and can compute
 * the time surface for new ones.
 * 
 * The time surface has a linear activation as described in (Maro et al., 2020).
 */
class LinearTimeSurface : public TimeSurfaceBase {

public:

    using TimeSurfaceBase::TimeSurfaceBase;

    std::pair<TimeSurfaceType, bool> compute(uint64_t t, uint16_t x, uint16_t y) const override;

    std::pair<TimeSurfaceType, bool> compute(const event& ev) const override;

    TimeSurfaceType sampleContext(uint64_t t) const override;

    void toStream(std::ostream& out) const override;

    void fromStream(std::istream& in) override;

};


/**
 * @brief Class that can compute linear time surfaces, with weighted output
 * 
 * This class keeps track of the time context for the current stream of events and can compute
 * the time surface for new ones.
 * 
 * The time surface has a linear activation as described in (Maro et al., 2020).
 * 
 * Output time surfaces and time context are weighted by a weight matrix.
 */
class WeightedLinearTimeSurface : public LinearTimeSurface {

public:

    /**
     * @copydoc TimeSurfaceBase::TimeSurfaceBase()
     */
    WeightedLinearTimeSurface();

    /**
     * @copydoc TimeSurfaceBase::TimeSurfaceBase(uint16_t,uint16_t,uint16_t,uint16_t,TimeSurfaceScalarType)
     * @param weightmatrix matrix used to weight the time surfaces
     * 
     * The weight matrix must have the same size as the context.
     */
    WeightedLinearTimeSurface(uint16_t width, uint16_t height, uint16_t Rx, uint16_t Ry, TimeSurfaceScalarType tau, const TimeSurfaceType& weightmatrix);

    /**
     * @copydoc LinearTimeSurface::compute(uint64_t,uint16_t,uint16_t) const
     * 
     * The output time surface is weighted.
     */
    std::pair<TimeSurfaceType, bool> compute(uint64_t t, uint16_t x, uint16_t y) const override;

    /**
     * @copydoc LinearTimeSurface::compute(const event&) const
     * 
     * The output time surface is weighted.
     */
    std::pair<TimeSurfaceType, bool> compute(const event& ev) const override;

    /**
     * @copydoc LinearTimeSurface::sampleContext
     * 
     * The sampled context is weighted.
     */
    TimeSurfaceType sampleContext(uint64_t t) const override;

    void toStream(std::ostream& out) const override;

    void fromStream(std::istream& in) override;

private:

    TimeSurfaceType weights;

    void setWeightMatrix(const TimeSurfaceType& weightmatrix);

};



/**
 * @brief Pool of time surface computations
 * 
 * This class holds a pool of time surfaces so that it can dispatch
 * events with different polarities to the appropriate time surface.
 * 
 */
class TimeSurfacePool : public virtual Streamable {

public:

    /**
     * @brief Construct a new pool of time surfaces
     * 
     * This constructor is provided only to create containers
     * with TimeSurfacePool instances or to load a pool from file.
     * It should not be used to create an usable pool.
     * 
     * See TimeSurfacePool::create.
     */
    TimeSurfacePool() {}

    /**
     * @brief Create a time surface pool
     * 
     * This function creates a pool of time surfaces, Forwarding arguments to
     * the time surface constructors.
     * 
     * @tparam TS time surface type
     * @tparam TSArgs types of the time surface constructor arguments
     * @param polarities numer of polarities (size of the pool)
     * @param tsargs arguments forwarded to the time surface constructor
     * @return the constructed pool
     */
    template <typename TS, typename... TSArgs>
    static TimeSurfacePool create(uint16_t polarities, TSArgs... tsargs) {

        TimeSurfacePool tsp;

        for (uint16_t i = 0; i < polarities; i++) {
            tsp.surfaces.push_back(TimeSurfacePtr(new TS(std::forward<TSArgs>(tsargs)...)));
        }

        return tsp;

    }

    /**
     * @brief Update the time context with a new event
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @param p polarity of the event
     */
    void update(uint64_t t, uint16_t x, uint16_t y, uint16_t p) {
        assert_polarity(p);
        surfaces[p]->update(t, x, y);
    }

    /**
     * @brief Update the time context with a new event
     * 
     * @param ev the new event
     */
    void update(const event& ev) {
        update(ev.t, ev.x, ev.y, ev.p);
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
     * @param p polarity of the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    std::pair<TimeSurfaceType, bool> compute(uint64_t t, uint16_t x, uint16_t y, uint16_t p) const {
        assert_polarity(p);
        return surfaces[p]->compute(t, x, y);
    }

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
    std::pair<TimeSurfaceType, bool> compute(const event& ev) const {
        return compute(ev.t, ev.x, ev.y, ev.p);
    }

    /**
     * @brief Update the time context and compute the new surface
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @param p polarity of the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    std::pair<TimeSurfaceType, bool> updateAndCompute(uint64_t t, uint16_t x, uint16_t y, uint16_t p) {
        update(t, x, y, p);
        return compute(t, x, y, p);
    }

    /**
     * @brief Update the time context and compute the new surface
     * 
     * @param ev the event
     * @return a std::pair with the computed time surface and whether the surface is valid or not
     */
    std::pair<TimeSurfaceType, bool> updateAndCompute(const event& ev){
        update(ev.t, ev.x, ev.y, ev.p);
        return compute(ev.t, ev.x, ev.y, ev.p);
    }

    /**
     * @brief Returns the size of the context
     * 
     * @return {width, height}
     */
    std::pair<uint16_t, uint16_t> getSize() const {
        return surfaces[0]->getSize();
    }

    /**
     * @brief Reset the time surfaces
     * 
     * This method resets the time surfaces.
     * It should be called before every stream of events.
     */
    void reset() {
        for (auto& ts : surfaces) {
            ts->reset();
        }
    }

    /**
     * @brief Access a time surface with boundaries check
     * 
     * This method will throw an invalid_argument exception if the index of the
     * surfaces exceeds the number of actual time surfaces.
     * 
     * @param idx index of the time surface
     * @return reference to the time surface
     */
    TimeSurfacePtr& getSurface(size_t idx) {
        assert_polarity(idx);
        return surfaces[idx];
    }

    /**
     * @brief Access a time surface with boundaries check
     * 
     * This method will throw an invalid_argument exception if the index of the
     * surfaces exceeds the number of actual time surfaces.
     * 
     * @param idx index of the time surface
     * @return reference to the time surface
     */
    const TimeSurfacePtr& getSurface(size_t idx) const {
        assert_polarity(idx);
        return surfaces[idx];
    }

    /**
     * @brief Sample and decay all temporal contexts from the pool
     * 
     * This functions applies the decay to the all temporal contexts and returns them.
     * 
     * @param t sample time
     * @return vector of decayed temporal contexts
     */
    std::vector<TimeSurfaceType> sampleContexts(uint64_t t) {
        std::vector<TimeSurfaceType> ret;
        for (const auto& ts : surfaces) {
            ret.push_back(ts->sampleContext(t));
        }
        return ret;
    }

    /**
     * @brief Get the number of surfaces in the pool
     * 
     * @return number of surfaces
     */
    size_t getNumSurfaces() {
        return surfaces.size();
    }

    /**
     * @copydoc Streamable::toStream
     * 
     * Save paramaters for all time surfaces to the stream.
     */
    void toStream(std::ostream& out) const override;

    /**
     * @copydoc Streamable::fromStream
     * 
     * Load parameters for all time surfaces from the stream.
     */
    void fromStream(std::istream& in) override;

private:
    std::vector<TimeSurfacePtr> surfaces;

    void assert_polarity(uint16_t p) const {
        if (p >= surfaces.size()) {
            throw std::invalid_argument("Polarity index exceeded: " + std::to_string(p) + ". Layer has only " + std::to_string(surfaces.size()) + " input polarities.");
        }
    }

};


/**
 * @brief Shorthand for TimeSurfacePool::create
 * 
 * @tparam TS time surface type
 * @tparam TSArgs types of the time surface constructor arguments
 * @param polarities numer of polarities (size of the pool)
 * @param tsargs arguments forwarded to the time surface constructor
 * @return the constructed pool
 */
template <typename TS, typename... TSArgs>
TimeSurfacePool create_pool(uint16_t polarities, TSArgs... tsargs) {
    return TimeSurfacePool::create<TS>(polarities, std::forward<TSArgs>(tsargs)...);
}

}

#endif