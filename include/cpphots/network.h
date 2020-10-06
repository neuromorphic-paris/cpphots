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

    /**
     * @brief Construct a new Network object
     * 
     * This constructor builds a hierarchy of layers as described in (Lagorce et al., 2017),
     * that is, every layer uses the same parameters of the previous one, multiplied by a growth factor.
     * 
     * @param width width of the full time context for the surfaces
     * @param height height of the full time context for the surfaces
     * @param polarities number of input polarities
     * @param num_layers
     * @param Rx1 horizontal size of the window on which surfaces are computed for the first layer
     * @param Ry1 vertical size of the window on which surfaces are computed for the first layer
     * @param K_R growth factor for the windows
     * @param tau1 time constant of the surfaces for the first layer
     * @param K_tau growth factor for the time constant
     * @param N1 number of features for the first layer
     * @param K_N growth factor for the number of features
     */
    Network(uint16_t width, uint16_t height, uint16_t polarities,
            uint16_t num_layers,
            uint16_t Rx1, uint16_t Ry1, uint16_t K_R,
            float tau1, float K_tau,
            uint16_t N1, uint16_t K_N);

    /**
     * @brief Construct a new Network object
     * 
     * This constructor builds a sequence of layers with completely customizable parameters.
     * 
     * @param width width of the full time context for the surfaces
     * @param height height of the full time context for the surfaces
     * @param polarities number of input polarities
     * @param Rx horizontal sizes of the windows on which surfaces are computed for each layer
     * @param Ry vertical sizes of the windows on which surfaces are computed for each layer
     * @param tau time constants of the surfaces for each layer
     * @param N numbers of features for each layer
     */
    Network(uint16_t width, uint16_t height, uint16_t polarities,
            const std::vector<uint16_t>& Rx, const std::vector<uint16_t>& Ry,
            const std::vector<float> tau,
            const std::vector<uint16_t> N);

    /**
     * @brief Process an event
     * 
     * Process the event through all the layers of the network, emitting the event from the last layer.
     * If the input event produces a non valid surface in any layer, also the output event is not valid.
     * 
     * If learning is enabled in some layer, this function will also update its prototypes.
     * 
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @param t time of the event
     * @param p polarity of the event
     * @return a std::pair with the new emitted event and whether the event is valid or not
     */
    std::pair<event, bool> process(uint16_t x, uint16_t y, uint64_t t, uint16_t p);

    /**
     * @brief Process an event
     * 
     * Process the event through all the layers of the network, emitting the event from the last layer.
     * If the input event produces a non valid surface in any layer, also the output event is not valid.
     * 
     * If learning is enabled in some layer, this function will also update its prototypes.
     * 
     * @param ev the event
     * @return a std::pair with the new emitted event and whether the event is valid or not
     */
    inline std::pair<event, bool> process(const event& ev) {
        return process(ev.x, ev.y, ev.t, ev.p);
    }

    /**
     * @brief Process a stream of events
     * 
     * Process a stream of events through all the layers and emit a the stream of events produced by the last layer.
     * Only valid events are emitted.
     * 
     * This method resets the time surfaces before processing the events.
     *
     * If learning is enabled in some layer, this function will also update matching prototypes.
     * 
     * @param events a stream of events
     * @return a new stream of valid events
     */
    Events process(const Events& events);

    /**
     * @brief Process a vector of stream of events
     * 
     * Process each stream of events through all the layers and emit a stream of events, as produced by the last layer, for every one of the input streams.
     * Only valid events are emitted.
     * 
     * This method resets the time surfaces before processing the events.
     *
     * If learning is enabled in some layer, this function will also update matching prototypes.
     * 
     * @param event_streams a vector of stream of events
     * @return a new vector of streams of valid events
     */
    std::vector<Events> process(const std::vector<Events>& event_streams);

    /**
     * @brief Get the number of layers in the network
     * 
     * @return the number of layers in the network
     */
    size_t getNumLayers() const;

    /**
     * @brief Get the number of input polarities
     * 
     * @return the number of input polarities
     */
    unsigned int getInputPolarities() const;

    /**
     * @brief Access a Layer
     * 
     * @param l index of the Layer to access
     * @return reference to the Layer
     */
    Layer& getLayer(size_t l);

    /**
     * @brief Get the histogram of prototypes activations
     * 
     * This method returns the histogram of prototypes activations as computed by the last layer.
     * 
     * @return the histogram of activations
     */
    std::vector<uint32_t> getLastHistogram() const;

    /**
     * @brief Reset the network
     * 
     * Reset the time surfaces and histogram of activations in every Layer.
     */
    void resetLayers();

    /**
     * @brief Enable or disable learning
     * 
     * This affects whether the prototypes are updated when process is called or not, for all layers.
     * 
     * @param enable true if learning should be active, false otherwise
     */
    void toggleLearningAll(bool enable = true);

    /**
     * @brief Enable or disable learning
     * 
     * This affects whether the prototypes are updated when process is called or not, for a specific layer.
     * All other layers will be set to the opposite learning mode.
     * 
     * @param l index of the layer
     * @param enable true if learning should be active, false otherwise
     */
    void toggleLearningLayer(size_t l, bool enable = true);

    /**
     * @brief Get network description
     * 
     * @return a string describing the parameters of the network
     */
    std::string getDescription() const;

    /**
     * @brief Stream insertion operator for Network
     * 
     * Recursively insert layers into the stream
     * 
     * @param out output stream
     * @param network Network to insert
     * @return output stream
     */
    friend std::ostream& operator<<(std::ostream& out, const Network& network);

    /**
     * @brief Stream extraction operator for Network
     * 
     * Recursively extract network layers. Previous layers are overwritten.
     * 
     * @param in input stream
     * @param network Network where to extract into
     * @return input stream
     */
    friend std::istream& operator>>(std::istream& in, Network& network);

private:
    std::vector<Layer> layers;
    unsigned int inputPolarities;

};

}

#endif