/**
 * @file layer.h
 * @brief Implementation of HOTS layers
 */
#ifndef LAYER_H
#define LAYER_H

#include <type_traits>

#include "events_utils.h"
#include "clustering.h"
#include "layer_traits.h"
#include "layer_modifiers.h"


namespace cpphots {

/**
 * @brief Interface for a generic layer
 */
class LayerBase {

public:

    /**
     * @brief Process an event and return zero or more events
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @param p polarity of the event
     * @param skip_check if true consider the event as valid
     * @return the emitted events, possibly empty
     */
    virtual Events process(uint64_t t, uint16_t x, uint16_t y, uint16_t p, bool skip_check = false) = 0;

    /**
     * @brief Process an event and return zero or more events
     * 
     * @param ev the event
     * @param skip_check if true consider the event as valid
     * @return the emitted events, possibly empty
     */
    virtual Events process(const event& ev, bool skip_check = false) = 0;

    /**
     * @brief Resets the layer
     */
    virtual void reset() = 0;

};


/**
 * @brief Composable Layer
 * 
 * This class implements a HOTS layer that can be built from
 * an arbitrary number of components.
 * 
 * One of these components should behave like a TimeSurfacesPool,
 * by providing an updateAndCompute method that computes a time surface.
 * 
 * If one of the components is a subclass of ClustererBase, clustering
 * is also performed on every event.
 * 
 * @tparam T types of the components
 */
template <typename... T>
class Layer : public LayerBase, public T... {

public:

    /**
     * @brief Construct a new Layer object
     * 
     * This constructor is provided only to create containers
     * with Layer instances or to load a Layer from file.
     * It should not be used to create an usable layer.
     */
    Layer() {}

    /**
     * @brief Construct a new Layer object
     * 
     * Create a new layer object by copying the components provided.
     * 
     * @param v the components to be assembled into a layer
     */
    template < typename... U >
    Layer(U &&... v) : T(std::forward < U >(v))... {}

    Events process(uint64_t t, uint16_t x, uint16_t y, uint16_t p, bool skip_check = false) override {

        // NOTE: time surfaces are required
        // compute the current time surface
        auto [surface, good] = this->updateAndCompute(t, x, y, p);

        // if the surface is not good we say it upstream
        if (!skip_check && !good) {
            return Events{};
        }

        // if there is a clustering algorithm we can use it
        if constexpr (std::is_base_of_v<ClustererBase, Layer>) {
            auto k = this->cluster(surface);

            if constexpr (std::is_base_of_v<ArrayLayer, Layer>) {
                return {event{t, k, y, 0}};
            }

            return {event{t, x, y, k}};
        }

        return Events{event{t, x, y, p}};

    }

    Events process(const event& ev, bool skip_check = false) override {
        return process(ev.t, ev.x, ev.y, ev.p, skip_check);
    }

    /**
     * @copydoc LayerBase::reset
     * 
     * Calls reset on all its supertypes that support it.
     */
    void reset() override {

        ([this]() {
            if constexpr (is_resettable_v<T>) {
                T::reset();
            }
        }(), ...);

    }

    /**
     * @brief Stream insertion operator for Layer
     * 
     * Calls operator<< on the supertypes that support it.
     * 
     * @param out output stream
     * @param layer Layer to insert
     * @return output stream
     */
    template <typename... TT>
    friend std::ostream& operator<<(std::ostream& out, const Layer<TT...>& layer);

    /**
     * @brief Stream extraction operator for Layer
     * 
     * Calls operator>> on the supertypes that support it.
     * Previous parameters are overwritten.
     * 
     * @param in input stream
     * @param layer Layer where to extract into
     * @return input stream
     */
    template <typename... TT>
    friend std::istream& operator>>(std::istream& in, Layer<TT...>& layer);

};

/**
 * @copydoc Layer::operator<<
 */
template <typename... T>
std::ostream& operator<<(std::ostream& out, const Layer<T...>& layer) {

    ([&]() {
    if constexpr (is_to_stream_writable_v<T>) {
        out << static_cast<const T&>(layer) << std::endl;
    }
    }(), ...);

    return out;

}

/**
 * @copydoc Layer::operator>>
 */
template <typename... T>
std::istream& operator>>(std::istream& in, Layer<T...>& layer) {

    ([&]() {
    if constexpr (is_from_stream_readable_v<T>) {
        in >> static_cast<T&>(layer);
    }
    }(), ...);

    return in;

}

/**
 * @brief Create a new Layer
 * 
 * Creates a new Layer by copying the components passed
 * 
 * @param v layer components
 * @return the constructed layer
 */
template <typename... T>
Layer<std::decay_t<T>...> create_layer(T&&... v) {
    return Layer<std::decay_t<T>...>{v...};
}


/**
 * @brief Initialize prototypes from a stream of events
 * 
 * @param initializer the initialization algorithm
 * @param ts_processer the time surface processed used to generate time surface from events
 * @param clusterer the clustere to be initialized
 * @param events the stream of events to be used
 * @param valid_only use only valid time surfaces for the initialization
 */
template <typename TSP,
          typename C>
void layerInitializePrototypes(const ClustererInitializerType& initializer, TSP& ts_processer, C& clusterer, const Events& events, bool valid_only = true) {

    // store all time surfaces
    ts_processer.reset();
    std::vector<TimeSurfaceType> time_surfaces;
    for (auto& ev : events) {
        auto surface_good = ts_processer.updateAndCompute(ev);
        if (valid_only && surface_good.second) {
            time_surfaces.push_back(surface_good.first);
        } else if (!valid_only) {
            time_surfaces.push_back(surface_good.first);
        }
    }

    if (time_surfaces.size() < clusterer.getNumClusters()) {
        throw std::runtime_error("Not enough good events to initialize prototypes.");
    }

    initializer(clusterer, time_surfaces);

}

/**
 * @brief Initialize prototypes from a vector of streams of events
 * 
 * @param initializer the initialization algorithm
 * @param ts_processer the time surface processed used to generate time surface from events
 * @param clusterer the clustere to be initialized
 * @param event_streams the vector of streams of events to be used
 * @param valid_only use only valid time surfaces for the initialization
 */
template <typename TSP>
void layerInitializePrototypes(const ClustererInitializerType& initializer, TSP& ts_processer, ClustererBase& clusterer, const std::vector<Events>& event_streams, bool valid_only = true) {

    // store all time surfaces
    std::vector<TimeSurfaceType> time_surfaces;
    for (auto& stream : event_streams) {
        ts_processer.reset();
        for (auto& ev : stream) {
            auto surface_good = ts_processer.updateAndCompute(ev);
            if (valid_only && surface_good.second) {
                time_surfaces.push_back(surface_good.first);
            } else if (!valid_only) {
                time_surfaces.push_back(surface_good.first);
            }
        }
    }

    if (time_surfaces.size() < clusterer.getNumClusters()) {
        throw std::runtime_error("Not enough good events to initialize prototypes.");
    }

    initializer(clusterer, time_surfaces);

}

}

#endif