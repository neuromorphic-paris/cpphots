#ifndef LAYER_H
#define LAYER_H

#include <type_traits>

#include "events_utils.h"
#include "clustering.h"
#include "layer_traits.h"


namespace cpphots {


class LayerBase {

public:
    virtual Events process(uint64_t t, uint16_t x, uint16_t y, uint16_t p, bool skip_check = false) = 0;

    virtual Events process(const event& ev, bool skip_check = false) = 0;

    virtual void reset() = 0;

};


// TODO: change name
struct ArrayLayer {};


template <typename... T>
class Layer : public LayerBase, public T... {

public:

    Layer() {}

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

    void reset() override {

        ([this]() {
            if constexpr (is_resettable_v<T>) {
                T::reset();
            }
        }(), ...);

    }

    template <typename... TT>
    friend std::ostream& operator<<(std::ostream& out, const Layer<TT...>& layer);

    template <typename... TT>
    friend std::istream& operator>>(std::istream& in, Layer<TT...>& layer);

};

template <typename... T>
std::ostream& operator<<(std::ostream& out, const Layer<T...>& layer) {

    ([&]() {
    if constexpr (is_to_stream_writable_v<T>) {
        out << static_cast<const T&>(layer) << std::endl;
    }
    }(), ...);

    return out;

}

template <typename... T>
std::istream& operator>>(std::istream& in, Layer<T...>& layer) {

    ([&]() {
    if constexpr (is_from_stream_readable_v<T>) {
        in >> static_cast<T&>(layer);
    }
    }(), ...);

    return in;

}


template <typename... T>
Layer<std::decay_t<T>...> create_layer(T&&... v) {
    return Layer<std::decay_t<T>...>{v...};
}

/**
 * @brief Initialize prototypes from a stream of events
 * 
 * @param initializer the initialization algorithm
 * @param layer the layer to be initialized
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
 * @param layer the layer to be initialized
 * @param event_streams the vector of streams of events to be used
 * @param valid_only use only valid time surfaces for the initialization
 */
template <typename TSP,
          typename C>
void layerInitializePrototypes(const ClustererInitializerType& initializer, TSP& ts_processer, C& clusterer, const std::vector<Events>& event_streams, bool valid_only = true) {

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