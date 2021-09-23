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
     * Creates a new Layer with the specified components.
     * 
     * @param tspool time surface pool calculator
     * @param clusterer clusterer
     * @param remapper event remapper
     * @param supercell supercell modifier
     */
    void createLayer(interfaces::TimeSurfacePoolCalculator* tspool = nullptr,
                   interfaces::Clusterer* clusterer = nullptr,
                   interfaces::EventRemapper* remapper = nullptr,
                   interfaces::SuperCell* supercell = nullptr);

    /**
     * @brief Append a layer to the network
     * 
     * @param layer the layer to be added
     */
    void addLayer(const Layer& layer);

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
     * @param pos index of the layer to access
     * @return reference to the layer
     */
    Layer& getLayer(size_t pos);

    /**
     * @brief Access a layer
     * 
     * @param pos index of the layer to access
     * @return reference to the layer
     */
    const Layer& getLayer(size_t pos) const;

    /**
     * @brief Access a layer
     * 
     * @param pos index of the layer to access
     * @return reference to the layer
     */
    Layer& operator[](size_t pos);

    /**
     * @brief Access a layer
     * 
     * @param pos index of the layer to access
     * @return reference to the layer
     */
    const Layer& operator[](size_t pos) const;

    /**
     * @brief Access the last layer
     * 
     * @return reference to the layer
     */
    Layer& back();

    /**
     * @brief Access the last layer
     * 
     * @return reference to the layer
     */
    const Layer& back() const;

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

    /**
     * @brief Iterator over layers
     */
    using iterator = std::vector<Layer>::iterator;

    /**
     * @brief Const iterator over layers
     */
    using const_iterator = std::vector<Layer>::const_iterator;

    /**
     * @brief Returns an iterator to the first layer
     * 
     * @return iterator
     */
    iterator begin() noexcept;

    /**
     * @brief Returns an iterator to the first layer
     * 
     * @return iterator
     */
    const_iterator begin() const noexcept;

    /**
     * @brief Returns an iterator to the first layer
     * 
     * @return iterator
     */
    const_iterator cbegin() const noexcept;

    /**
     * @brief Returns an iterator to the layer following the last layer
     * 
     * @return iterator
     */
    iterator end() noexcept;

    /**
     * @brief Returns an iterator to the layer following the last layer
     * 
     * @return iterator
     */
    const_iterator end() const noexcept;

    /**
     * @brief Returns an iterator to the layer following the last layer
     * 
     * @return iterator
     */
    const_iterator cend() const noexcept;

private:
    std::vector<Layer> layers;

};

Network operator+(const Network& n1, const Network& n2);

}

#endif