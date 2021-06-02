/**
 * @file layer.h
 * @brief Implementation of HOTS layers
 */
#ifndef CPPHOTS_LAYER_H
#define CPPHOTS_LAYER_H

#include <type_traits>
#include <memory>

#include "types.h"
#include "clustering.h"
#include "layer_traits.h"
#include "layer_modifiers.h"
#include "interfaces/streamable.h"

/**
 * @brief Main cpphots namespace
 */
namespace cpphots {

/**
 * @brief Interface for a generic layer
 */
class LayerBase : public virtual interfaces::Streamable {

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
 * @brief Alias for pointer to layer
 */
using LayerPtr = std::shared_ptr<LayerBase>;


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
    Layer(U &&... v) : T(std::forward < U >(v))... {
        assert_sizes();
    }

    Events process(uint64_t t, uint16_t x, uint16_t y, uint16_t p, bool skip_check = false) override {

        // NOTE: time surfaces are required
        // compute the current time surface
        auto [surface, good] = this->updateAndCompute(t, x, y, p);

        // if the surface is not good we say it upstream
        if (!skip_check && !good) {
            return Events{};
        }

        std::vector<TimeSurfaceType> surfaces;
        std::vector<std::pair<uint16_t, uint16_t>> coords;

        // check averaging
        if constexpr (std::is_base_of_v<SuperCell, Layer>) {
            coords = this->findCells(x, y);
            for (auto [cx, cy] : coords) {
                if constexpr (std::is_base_of_v<SuperCellAverage, Layer>) {
                    surfaces.push_back(this->averageTS(surface, cx, cy));
                } else {
                    surfaces.push_back(surface);
                }
            }
        } else {
            surfaces.push_back(surface);
            coords.push_back({x, y});
        }

        // if there is a clustering algorithm we can use it
        if constexpr (std::is_base_of_v<ClustererBase, Layer>) {

            Events retevents;

            for (size_t i = 0; i < surfaces.size(); i++) {

                auto& surface = surfaces[i];
                auto [x, y] = coords[i];

                auto k = this->cluster(surface);

                if constexpr (std::is_base_of_v<EventRemapper, Layer>) {
                    retevents.push_back(this->remapEvent(event{t, x, y, p}, k));
                } else {
                    retevents.push_back(event{t, x, y, k});
                }

            }

            return retevents;
        }

        Events retevents;
        for (auto [ex, ey] : coords) {
            retevents.push_back(event{t, ex, ey, p});
        }

        return retevents;

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
     * @copydoc Streamable::toStream
     * 
     * Calls toStream on the supertypes that support it.
     */
    void toStream(std::ostream& out) const override {

        writeMetacommand(out, "LAYERBEGIN");

        ([&]() {
        if constexpr (std::is_base_of_v<Streamable, T>) {
            T::toStream(out);
            out << std::endl;
        }
        }(), ...);

        writeMetacommand(out, "LAYEREND");

    }

    /**
     * @copydoc Streamable::fromStream
     * 
     * Calls fromStream on the supertypes that support it.
     */
    void fromStream(std::istream& in) override {

        matchMetacommandOptional(in, "LAYERBEGIN");

        ([&]() {
        if constexpr (std::is_base_of_v<Streamable, T>) {
            T::fromStream(in);
        }
        }(), ...);

        matchMetacommandOptional(in, "LAYEREND");

    }

private:
    void assert_sizes() const {
        int w = -1;
        int h = -1;
        ([this, &w, &h]() {
            if constexpr (has_size_v<T>) {
                auto [_w, _h] = T::getSize();
                if (w == -1) {
                    w = _w;
                    h = _h;
                } else if (w != _w || h != _h) {
                    throw std::invalid_argument("Size mismatch in Layer: trying to create a Layer with components of different sizes.");
                }
            }
        }(), ...);
    }

};

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
 * @brief Create a new layer
 * 
 * Creates a new Layer by copying the components passed
 * 
 * @param v layer components
 * @return pointer to layer
 */
template <typename... T>
LayerPtr create_layer_ptr(T&&... v) {
    return std::make_shared<Layer<std::decay_t<T>...>>(v...);
}

/**
 * @brief Initialize prototypes from a stream of events
 * 
 * @param initializer the initialization algorithm
 * @param ts_processer the time surface processed used to generate time surface from events
 * @param clusterer the clusterer to be initialized
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
 * @param clusterer the clusterer to be initialized
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