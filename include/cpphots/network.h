/**
 * @file network.h
 * @brief Implementation of a full HOTS network
 */

#ifndef HOTS_H
#define HOTS_H

#include <cstdint>
#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <functional>

#include "layer.h"
#include "events_utils.h"


namespace cpphots {

/**
 * @brief A multi-layered HOTS network
 * 
 * This class can process events through a sequence of layers.
 * 
 * The layers of the network must be properly initialized before processing events.
 * 
 * Once the layers are initialized, traning of the whole network can be achieved by setting toggleLearning to true and then processing events.
 */
class Network {

public:

    Network();

    void addLayer(LayerBase& layer);

    /**
     * @brief Process an event
     * 
     * Process the event through all the layers of the network, emitting the event from the last layer.
     * If the input event produces a non valid surface in any layer, also the output event is not valid.
     * 
     * If learning is enabled in some layer, this function will also update its prototypes.
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @param p polarity of the event
     * @return the new emitted event, possibly invalid
     */
    Events process(uint64_t t, uint16_t x, uint16_t y, uint16_t p, bool skip_check = false);

    /**
     * @brief Process an event
     * 
     * Process the event through all the layers of the network, emitting the event from the last layer.
     * If the input event produces a non valid surface in any layer, also the output event is not valid.
     * 
     * If learning is enabled in some layer, this function will also update its prototypes.
     * 
     * @param ev the event
     * @return the new emitted event, possibly invalid
     */
    Events process(const event& ev, bool skip_check = false);

    /**
     * @brief Get the number of layers in the network
     * 
     * @return the number of layers in the network
     */
    size_t getNumLayers() const;

    /**
     * @brief Access a Layer
     * 
     * @param pos index of the Layer to access
     * @return reference to the Layer
     */
    template <typename T = LayerBase>
    T& getLayer(size_t pos) {
        return dynamic_cast<T&>(layers[pos].get());
    }

    /**
     * @brief Access a Layer
     * 
     * @param pos index of the Layer to access
     * @return reference to the Layer
     */
    template <typename T = LayerBase>
    const T& getLayer(size_t pos) const {
        return dynamic_cast<const T&>(layers[pos].get());
    }

    LayerBase& operator[](size_t pos);

    const LayerBase& operator[](size_t pos) const;

    template <typename T = LayerBase>
    T& back() {
        return *(view<T>().back());
    }

    template <typename T = LayerBase>
    const T& back() const {
        return *(view<T>().back());
    }

    template <typename T>
    std::vector<T*> view() {
        std::vector<T*> ret;
        for (auto& l : layers) {
            T* lp = dynamic_cast<T*>(&(l.get()));
            if (lp) {
                ret.push_back(lp);
            }
        }
        return ret;
    }

    template <typename T>
    std::vector<T*> viewFull() {
        std::vector<T*> ret;
        for (auto& l : layers) {
            ret.push_back(dynamic_cast<T*>(&(l.get())));
        }
        return ret;
    }


    /**
     * @brief Reset the network
     * 
     * Reset the time surfaces and histogram of activations in every Layer.
     */
    void reset();

private:
    std::vector<std::reference_wrapper<LayerBase>> layers;

};

}

#endif