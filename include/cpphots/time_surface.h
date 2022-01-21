/**
 * @file time_surface.h
 * @brief Time surface implementation
 */
#ifndef CPPHOTS_TIME_SURFACE_H
#define CPPHOTS_TIME_SURFACE_H

#include <ostream>
#include <istream>
#include <memory>

#include "assert.h"
#include "types.h"
#include "interfaces/time_surface.h"


namespace cpphots {


/**
 * @brief Base class for classes that can compute time surfaces
 * 
 * This class keeps track of the time context for the current stream of events,
 * but it's up to the suclasses to compute the time surfaces.
 */
class TimeSurfaceBase : public interfaces::TimeSurfaceCalculator {

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

    void update(uint64_t t, uint16_t x, uint16_t y) override;

    void update(const event& ev) override {
        update(ev.t, ev.x, ev.y);
    }

    std::pair<TimeSurfaceType, bool> updateAndCompute(uint64_t t, uint16_t x, uint16_t y) override {
        update(t, x, y);
        return compute(t, x, y);
    }

    std::pair<TimeSurfaceType, bool> updateAndCompute(const event& ev) override {
        update(ev.t, ev.x, ev.y);
        return compute(ev.t, ev.x, ev.y);
    }

    const TimeSurfaceType& getFullContext() const override {
        return context;
    }

    TimeSurfaceType getContext() const override;

    void reset() override;

    std::pair<uint16_t, uint16_t> getSize() const override;

    uint16_t getWx() const override {
        return Wx;
    }

    uint16_t getWy() const override {
        return Wy;
    }

    /**
     * @copydoc interfaces::Streamable::toStream
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
 * @brief Class that can compute linear time surfaces
 * 
 * This class keeps track of the time context for the current stream of events and can compute
 * the time surface for new ones.
 * 
 * The time surface has a linear activation as described in (Maro et al., 2020).
 */
class LinearTimeSurface : public interfaces::Clonable<LinearTimeSurface, TimeSurfaceBase> {

public:

    using Clonable::Clonable;

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
class WeightedLinearTimeSurface : public interfaces::Clonable<WeightedLinearTimeSurface, LinearTimeSurface> {

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
class TimeSurfacePool : public interfaces::Clonable<TimeSurfacePool, interfaces::TimeSurfacePoolCalculator> {

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
     * @brief Destroy the TimeSurfacePool
     */
    ~TimeSurfacePool();

    /**
     * @brief Copy constructor
     * 
     * @param other pool to be copied
     */
    TimeSurfacePool(const TimeSurfacePool& other);

    /**
     * @brief Move constructor
     * 
     * @param other pool to be copied
     */
    TimeSurfacePool(TimeSurfacePool&& other);

    /**
     * Copy assignment
     * 
     * @param other pool to be copied
     */
    TimeSurfacePool& operator=(const TimeSurfacePool& other);

    /**
     * Move assignment
     * 
     * @param other pool to be moved
     */
    TimeSurfacePool& operator=(TimeSurfacePool&& other);

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
     * @brief Create pointer to a new time surface pool
     * 
     * This function creates a pool of time surfaces, Forwarding arguments to
     * the time surface constructors.
     * 
     * @tparam TS time surface type
     * @tparam TSArgs types of the time surface constructor arguments
     * @param polarities numer of polarities (size of the pool)
     * @param tsargs arguments forwarded to the time surface constructor
     * @return pointer to the constructed pool
     */
    template <typename TS, typename... TSArgs>
    static TimeSurfacePool* create_ptr(uint16_t polarities, TSArgs... tsargs) {

        TimeSurfacePool* tsp = new TimeSurfacePool();

        for (uint16_t i = 0; i < polarities; i++) {
            tsp->surfaces.push_back(TimeSurfacePtr(new TS(std::forward<TSArgs>(tsargs)...)));
        }

        return tsp;

    }

    void update(uint64_t t, uint16_t x, uint16_t y, uint16_t p) override {
        cpphots_assert(p < surfaces.size());
        surfaces[p]->update(t, x, y);
    }

    void update(const event& ev) override {
        update(ev.t, ev.x, ev.y, ev.p);
    }

    std::pair<TimeSurfaceType, bool> compute(uint64_t t, uint16_t x, uint16_t y, uint16_t p) const override {
        cpphots_assert(p < surfaces.size());
        return surfaces[p]->compute(t, x, y);
    }

    std::pair<TimeSurfaceType, bool> compute(const event& ev) const override {
        return compute(ev.t, ev.x, ev.y, ev.p);
    }

    std::pair<TimeSurfaceType, bool> updateAndCompute(uint64_t t, uint16_t x, uint16_t y, uint16_t p) override {
        update(t, x, y, p);
        return compute(t, x, y, p);
    }

    std::pair<TimeSurfaceType, bool> updateAndCompute(const event& ev) override {
        update(ev.t, ev.x, ev.y, ev.p);
        return compute(ev.t, ev.x, ev.y, ev.p);
    }

    std::pair<uint16_t, uint16_t> getSize() const override {
        return surfaces[0]->getSize();
    }

    void reset() override {
        for (auto& ts : surfaces) {
            ts->reset();
        }
    }

    TimeSurfacePtr& getSurface(size_t idx) override {
        cpphots_assert(idx < surfaces.size());
        return surfaces[idx];
    }

    const TimeSurfacePtr& getSurface(size_t idx) const override {
        cpphots_assert(idx < surfaces.size());
        return surfaces[idx];
    }

    std::vector<TimeSurfaceType> sampleContexts(uint64_t t) const override {
        std::vector<TimeSurfaceType> ret;
        for (const auto& ts : surfaces) {
            ret.push_back(ts->sampleContext(t));
        }
        return ret;
    }

    size_t getNumSurfaces() const override {
        return surfaces.size();
    }

    /**
     * @copydoc interfaces::Streamable::toStream
     * 
     * Save paramaters for all time surfaces to the stream.
     */
    void toStream(std::ostream& out) const override;

    /**
     * @copydoc interfaces::Streamable::fromStream
     * 
     * Load parameters for all time surfaces from the stream.
     */
    void fromStream(std::istream& in) override;

private:
    std::vector<TimeSurfacePtr> surfaces;

    void delete_surfaces();

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

/**
 * @brief Shorthand for TimeSurfacePool::create_ptr
 * 
 * @tparam TS time surface type
 * @tparam TSArgs types of the time surface constructor arguments
 * @param polarities numer of polarities (size of the pool)
 * @param tsargs arguments forwarded to the time surface constructor
 * @return pointer to the constructed pool
 */
template <typename TS, typename... TSArgs>
TimeSurfacePool* create_pool_ptr(uint16_t polarities, TSArgs... tsargs) {
    return TimeSurfacePool::create_ptr<TS>(polarities, std::forward<TSArgs>(tsargs)...);
}

}

#endif