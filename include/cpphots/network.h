/**
 * @file network.h
 * @brief Implementation of a full HOTS network
 */

#ifndef CPPHOTS_NETWORK_H
#define CPPHOTS_NETWORK_H

#include <cstdint>
#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <functional>

#include "types.h"
#include "layer.h"
#include "interfaces/streamable.h"


namespace cpphots {

/**
 * @brief A multi-layered HOTS network
 * 
 * This class can process events through a sequence of layers.
 * 
 * A Network owns no layers, only references to them.
 */
class Network : public interfaces::Streamable {

public:

    /**
     * @brief Construct a new Network object
     * 
     * The network has no layers and they must be added via the Network::addLayer method.
     */
    Network();

    /**
     * @brief Create a new layer and append it to the network
     * 
     * Creates a new Layer by copying the components passed
     * 
     * @param v layer components
     */
    template <typename... T>
    void addLayer(T&&... v) {
        layers.push_back(create_layer_ptr(std::forward<T>(v)...));
    }

    /**
     * @brief Append a layer to the network
     * 
     * @param layer the layer to be added
     */
    void addLayer(LayerPtr layer);

    /**
     * @brief Process an event
     * 
     * Process the event through all the layers of the network, emitting events from the last layer.
     * If the input event produces a non valid surface in any layer, the output events vector might be empty.
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @param p polarity of the event
     * @param skip_check if true consider the event as valid
     * @return the new emitted events, possibly empty
     */
    Events process(uint64_t t, uint16_t x, uint16_t y, uint16_t p, bool skip_check = false);

    /**
     * @brief Process an event
     * 
     * Process the event through all the layers of the network, emitting events from the last layer.
     * If the input event produces a non valid surface in any layer, also the output events might be empty.
     * 
     * @param ev the event
     * @param skip_check if true consider the event as valid
     * @return the new emitted events, possibly empty
     */
    Events process(const event& ev, bool skip_check = false);

    /**
     * @brief Get the number of layers in the network
     * 
     * @return the number of layers in the network
     */
    size_t getNumLayers() const;

    /**
     * @brief Access a layer
     * 
     * This function can be used to dynamically cast
     * a layer into another type.
     * 
     * @tparam T type to cast the layer into
     * @param pos index of the layer to access
     * @return reference to the layer
     */
    template <typename T = LayerBase>
    T& getLayer(size_t pos) {
        return dynamic_cast<T&>(*layers[pos]);
    }

    /**
     * @brief Access a layer
     * 
     * This function can be used to dynamically cast
     * a layer into another type.
     * 
     * @tparam T type to cast the layer into
     * @param pos index of the layer to access
     * @return reference to the layer
     */
    template <typename T = LayerBase>
    const T& getLayer(size_t pos) const {
        return dynamic_cast<const T&>(*layers[pos]);
    }

    /**
     * @brief Access a layer
     * 
     * @param pos index of the layer to access
     * @return reference to the layer
     */
    LayerBase& operator[](size_t pos);

    /**
     * @brief Access a layer
     * 
     * @param pos index of the layer to access
     * @return reference to the layer
     */
    const LayerBase& operator[](size_t pos) const;

    /**
     * @brief Access the last layer
     * 
     * This function can be used to dynamically cast
     * a layer into another type.
     * 
     * @tparam T type to cast the layer into
     * @return reference to the layer
     */
    template <typename T = LayerBase>
    T& back() {
        return *(view<T>().back());
    }

    /**
     * @brief Access the last layer
     * 
     * This function can be used to dynamically cast
     * a layer into another type.
     * 
     * @tparam T type to cast the layer into
     * @return reference to the layer
     */
    template <typename T = LayerBase>
    const T& back() const {
        return *(view<T>().back());
    }

    /**
     * @brief Dynamically cast layers to a new pointer type
     * 
     * This function will return only pointers to layers for
     * which the cast was successful.
     * 
     * @tparam T type to cast layers into
     * @return vector of pointers to layers
     */
    template <typename T>
    std::vector<T*> view() {
        std::vector<T*> ret;
        for (auto& l : layers) {
            T* lp = dynamic_cast<T*>(l.get());
            if (lp) {
                ret.push_back(lp);
            }
        }
        return ret;
    }

    /**
     * @brief Dynamically cast all layers to a new pointer type
     * 
     * This function will return pointers to layers for
     * which the cast was successful and nullptr for others,
     * in the order that they are in the Network.
     * 
     * @tparam T type to cast layers into
     * @return vector of pointers to layers
     */
    template <typename T>
    std::vector<T*> viewFull() {
        std::vector<T*> ret;
        for (auto& l : layers) {
            ret.push_back(dynamic_cast<T*>(l.get()));
        }
        return ret;
    }

    /**
     * @brief Create a subnetwork of the current Network
     * 
     * Returns a new network that shares layers [start:end) with this network.
     * Indexes can be negative (python-like) and end=0 means up to the end.
     * 
     * @param start start of interval
     * @param stop end of interval (not included)
     * @return the subnetwork
     */
    Network getSubnetwork(int start, int stop = 0) const;

    /**
     * @brief Reset the network
     * 
     * Call reset on every layer.
     */
    void reset();

    /**
     * @copydoc interfaces::Streamable::toStream
     * 
     * Save all layers to the stream.
     */
    void toStream(std::ostream& out) const override;

    /**
     * @copydoc interfaces::Streamable::fromStream
     * 
     * Load all layers to the stream.
     */
    void fromStream(std::istream& in) override;

private:
    std::vector<LayerPtr> layers;

};

}

#endif