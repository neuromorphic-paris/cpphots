/**
 * @file time_surface.h
 * @brief Time surface implementation
 */

#ifndef CPPHOTS_TIME_SURFACE_H
#define CPPHOTS_TIME_SURFACE_H

#include <ostream>
#include <istream>
#include <Eigen/Dense>
#include "events_utils.h"


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
 * @brief Class that can compute time surfaces
 * 
 * This class keeps track of the time context for the current stream of events and can compute
 * the time surface for new ones.
 * 
 * The time surface has a linear activation as described in (Maro et al., 2020).
 */
class LinearTimeSurface {

public:

    /**
     * @brief Construct a new Time Surface object
     * 
     * This constructor is provided only to create containers
     * with LinearTimeSurface instances or to load a time surface from file.
     * It should not be used to create an usable LinearTimeSurface object.
     */
    LinearTimeSurface();

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
    LinearTimeSurface(uint16_t width, uint16_t height, uint16_t Rx, uint16_t Ry, TimeSurfaceScalarType tau);

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
     * This function returns the full temporal context (including padding)
     * on which surfaces are computed.
     * 
     * You may want to use LinearTimeSurface::getContext() in normal circumnstances.
     * 
     * @return the temporal context
     */
    inline const TimeSurfaceType& getFullContext() const {
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
    TimeSurfaceType sampleContext(uint64_t t) const;

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
     * @param ts LinearTimeSurface to insert
     * @return output stream
     */
    friend std::ostream& operator<<(std::ostream& out, const LinearTimeSurface& ts);

    /**
     * @brief Stream extraction operator for Time Surfaces
     * 
     * Reads all parameters from the stream. Previous parameters are overwritten.
     * 
     * @param in input stream
     * @param ts LinearTimeSurface where to extract into
     * @return input stream
     */
    friend std::istream& operator>>(std::istream& in, LinearTimeSurface& ts);

private:

    TimeSurfaceType context;
    uint16_t width, height;
    uint16_t Rx, Ry;
    uint16_t Wx, Wy;
    TimeSurfaceScalarType tau;
    uint16_t min_events;

};


/**
 * @brief Interface for a pool of time surfaces
 */
class TimeSurfacePoolBase {

public:

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
    virtual LinearTimeSurface& getSurface(size_t idx) = 0;

    /**
     * @brief Access a time surface with boundaries check
     * 
     * This method will throw an invalid_argument exception if the index of the
     * surfaces exceeds the number of actual time surfaces.
     * 
     * @param idx index of the time surface
     * @return reference to the time surface
     */
    virtual const LinearTimeSurface& getSurface(size_t idx) const = 0;

    /**
     * @brief Sample and decay all temporal contexts from the pool
     * 
     * This functions applies the decay to the all temporal contexts and returns them.
     * 
     * @param t sample time
     * @return vector of decayed temporal contexts
     */
    virtual std::vector<TimeSurfaceType> sampleContexts(uint64_t t) = 0;

};


/**
 * @brief Pool of time surface computations
 * 
 * This class holds a pool of time surfaces so that it can dispatch
 * events with different polarities to the appropriate time surface.
 * 
 * @tparam TS type of time surface
 */
template <typename TS>
class TimeSurfacePool : public TimeSurfacePoolBase {

public:

    /**
     * @brief Alias for template type
     */
    using TimeSurface = TS;

    /**
     * @brief Construct a new pool of time surfaces
     * 
     * This constructor is provided only to create containers
     * with TimeSurfacePool instances or to load a pool from file.
     * It should not be used to create an usable pool.
     */
    TimeSurfacePool() {}

    /**
     * @brief Construct a new pool of time surfaces
     * 
     * Constructs a pool of time surfaces, Forwarding arguments to
     * the time surface constructors.
     * 
     * @tparam TSArgs types of the time surface constructor arguments
     * @param polarities numer of polarities (size of the pool)
     * @param tsargs arguments forwarded to the time surface constructor
     */
    template <typename... TSArgs>
    TimeSurfacePool(uint16_t polarities, TSArgs... tsargs) {

        for (uint16_t i = 0; i < polarities; i++) {
            surfaces.push_back(TS(std::forward<TSArgs>(tsargs)...));
        }

    }

    void update(uint64_t t, uint16_t x, uint16_t y, uint16_t p) override {
        assert_polarity(p);
        surfaces[p].update(t, x, y);
    }

    void update(const event& ev) override {
        update(ev.t, ev.x, ev.y, ev.p);
    }

    std::pair<TimeSurfaceType, bool> compute(uint64_t t, uint16_t x, uint16_t y, uint16_t p) const override {
        assert_polarity(p);
        return surfaces[p].compute(t, x, y);
    }

    std::pair<TimeSurfaceType, bool> compute(const event& ev) const override {
        return compute(ev.t, ev.x, ev.y, ev.p);
    }

    std::pair<TimeSurfaceType, bool> updateAndCompute(uint64_t t, uint16_t x, uint16_t y, uint16_t p) override {
        update(t, x, y, p);
        return compute(t, x, y, p);
    }

    std::pair<TimeSurfaceType, bool> updateAndCompute(const event& ev) override{
        update(ev.t, ev.x, ev.y, ev.p);
        return compute(ev.t, ev.x, ev.y, ev.p);
    }

    std::pair<uint16_t, uint16_t> getSize() const override {
        return surfaces[0].getSize();
    }

    void reset() override{
        for (auto& ts : surfaces) {
            ts.reset();
        }
    }

    TimeSurface& getSurface(size_t idx) override {
        assert_polarity(idx);
        return surfaces[idx];
    }

    const TimeSurface& getSurface(size_t idx) const override {
        assert_polarity(idx);
        return surfaces[idx];
    }

    std::vector<TimeSurfaceType> sampleContexts(uint64_t t) override {
        std::vector<TimeSurfaceType> ret;
        for (const auto& ts : surfaces) {
            ret.push_back(ts.sampleContext(t));
        }
        return ret;
    }

    /**
     * @brief Stream insertion operator for TimeSurfacePool
     * 
     * Insert parameters of the pool (and of all the timesurfaces included) on the stream.
     * 
     * @param out output stream
     * @param pool TimeSurfacePool to insert
     * @return output stream
     */
    template <typename TTS>
    friend std::ostream& operator<<(std::ostream& out, const TimeSurfacePool<TTS>& pool);

    /**
     * @brief Stream extraction operator for TimeSurfacePool
     * 
     * Reads parameters and prototypes for the pool (ad for all the timesurfaces included).
     * Previous parameters are overwritten.
     * 
     * @param in input stream
     * @param pool TimeSurfacePool where to extract into
     * @return input stream
     */
    template <typename TTS>
    friend std::istream& operator>>(std::istream& in, TimeSurfacePool<TTS>& pool);

private:
    std::vector<TS> surfaces;

    void assert_polarity(uint16_t p) const {
        if (p >= surfaces.size()) {
            throw std::invalid_argument("Polarity index exceeded: " + std::to_string(p) + ". Layer has only " + std::to_string(surfaces.size()) + " input polarities.");
        }
    }

};

/**
 * @copydoc TimeSurfacePool::operator<<
 */
template <typename TS>
std::ostream& operator<<(std::ostream& out, const TimeSurfacePool<TS>& pool) {

    out << pool.surfaces.size() << "\n";
    for (const auto& ts : pool.surfaces) {
        out << ts << "\n";
    }

    return out;

}

/**
 * @copydoc TimeSurfacePool::operator<<
 */
template <typename TS>
std::istream& operator>>(std::istream& in, TimeSurfacePool<TS>& pool) {

    pool.surfaces.clear();
    size_t n_surfaces;
    in >> n_surfaces;
    pool.surfaces.resize(n_surfaces);
    for (auto& sur : pool.surfaces) {
        in >> sur;
    }

    return in;

}

/**
 * @brief Alias for a time surface pool with linear time surfaces
 */
using LinearTimeSurfacePool = TimeSurfacePool<LinearTimeSurface>;

}

#endif