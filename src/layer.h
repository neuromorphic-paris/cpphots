/**
 * @file layer.h
 * @brief Implementation of HOTS layers and of layer initialization procedures
 */

#ifndef LAYER_H
#define LAYER_H

#include <cstdint>
#include <vector>
#include <string>

#include <eigen3/Eigen/Dense>

#include "time_surface.h"

#include "events_utils.h"


namespace cpphots {

/**
 * @brief A layer of HOTS
 * 
 * This class is capable of processing incoming events and emitting new events with different polarities, by extracting features.
 * The class also keeps track of the number of time a certain prototype has been activated, thus it can output an histogram of activations at any time.
 * 
 * Before processing events, the prototypes must be initialized using a subclass of LayerInitializer.
 * Calling process before having initialized the prototypes raises an exception.
 * 
 * After the initialization of the prototypes, learning is performed by enabling it via the toggleLearning method and processing events using process.
 * To freeze the prototypes and stop learning, simply disable it via toggleLearning.
 */
class Layer {

public:

    /**
     * @brief Construct a new Layer object
     * 
     * This constructor should never be used explicitly,
     * it is provided only to create containers with Time Surface instances.
     */
    Layer() {}

    /**
     * @brief Construct a new Layer object
     * 
     * This constructor will setup a number of time surfaces equal to the input polarities.
     * 
     * The constructor will not initialize the prototypes.
     * 
     * @param width width of the full time context for the surfaces
     * @param height height of the full time context for the surfaces
     * @param Rx horizontal size of the window on which surfaces are computed
     * @param Ry vertical size of the window on which surfaces are computed
     * @param tau time constant of the surfaces
     * @param polarities number of input polarities
     * @param features number of features extracted (equal to the number of output polarities)
     */
    Layer(uint16_t width, uint16_t height,
          uint16_t Rx, uint16_t Ry, float tau,
          uint16_t polarities, uint16_t features);
    
    /**
     * @brief Process an event
     * 
     * Process an event and emit a new event, depending on the closest prototype.
     * If the input event does not produce a valid time surface, then the output event is also not valid.
     * 
     * The closest prototype is choosen based on L2 distance.
     * 
     * If learning is enabled, this function will also update the matching prototype.
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
     * Process an event and emit a new event, depending on the closest prototype.
     * If the input event does not produce a valid time surface, then the output event is also not valid.
     * 
     * The closest prototype is choosen based on L2 distance.
     * 
     * If learning is enabled, this function will also update the matching prototype.
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
     * Process a stream of events and emit a new stream of events.
     * Only valid events are emitted.
     * 
     * This method resets the time surfaces before processing the events.
     *
     * If learning is enabled, this function will also update matching prototypes.
     * 
     * @param events a stream of events
     * @return a new stream of valid events
     */
    Events process(const Events& events);

    /**
     * @brief Process a vector of stream of events
     * 
     * Process each stream of events and emit a new stream of events for every one of the input streams.
     * Only valid events are emitted.
     * 
     * This method resets the time surfaces before processing every stream of events.
     *
     * If learning is enabled, this function will also update matching prototypes.
     * 
     * @param event_streams a vector of stream of events
     * @return a new vector of streams of valid events
     */
    std::vector<Events> process(const std::vector<Events>& event_streams);

    /**
     * @brief Get the number of output features
     * 
     * @return the number of output features
     */
    uint16_t getFeatures() const;

    /**
     * @brief Get the histogram of prototypes activations
     * 
     * @return the histogram of activations
     */
    std::vector<uint32_t> getHist() const;

    /**
     * @brief Get the list of prototypes
     * 
     * @return the list of prototypes 
     */
    std::vector<Eigen::ArrayXXf> getPrototypes() const;

    /**
     * @brief Reset the time surfaces
     * 
     * This method resets the time surfaces and the histogram of activations.
     * It should be called before every stream of events.
     */
    void resetSurfaces();

    /**
     * @brief Access a time surface with boundaries check
     * 
     * @param idx index of the time surface
     * @return reference to the time surface
     */
    inline TimeSurface& getSurface(unsigned int idx) {
        if (idx >= surfaces.size()) {
            throw std::invalid_argument("Polarity index exceeded: " + std::to_string(idx) + ". Layer has only " + std::to_string(surfaces.size()) + " input polarities.");
        }
        return surfaces[idx];
    }

    /**
     * @brief Enable or disable learning
     * 
     * This affects whether the prototypes are updated when process is called or not.
     * 
     * @param enable true if learning should be active, false otherwise
     * @return the previous learning state
     */
    bool toggleLearning(bool enable = true);

    /**
     * @brief Get layer description
     * 
     * @return a string describing the parameters of the Layer
     */
    std::string getDescription() const;

    /**
     * @brief Check prototype initialization
     * 
     * Prototypes are initialized if there is a number of prototypes equal to the number of features.
     * 
     * @return true if prototypes are initialized
     * @return false otherwise
     */
    inline bool isInitialized() const {
        return (prototypes.size() == features) && (prototypes_activations.size() == features);
    }

    /**
     * @brief Removes all prototypes
     */
    void clearPrototypes();

    /**
     * @brief Add a new prototype to the layer
     * 
     * This function should not be manually used, initialization should be done via a LayerInitializer.
     * 
     * If the Layer is already initialized, an exception is raised.
     * 
     * @param proto the prototype to add
     */
    void addPrototype(const Eigen::ArrayXXf& proto);

private:
    std::vector<Eigen::ArrayXXf> prototypes;
    std::vector<uint32_t> prototypes_activations;
    std::vector<TimeSurface> surfaces;
    uint16_t features;
    std::vector<uint32_t> hist;
    bool learning = true;
    std::string description;

};


/**
 * @brief Interface for classes capable of initializing a Layer
 * 
 * Subclasssed of this should be capable of initializing a Layer for a stream of events or a vector of such streams.
 * 
 * In the default implemetation, the initializePrototypes method only compute the valid time surfaces and pass them to initializationAlgorithm.
 */
class LayerInitializer {

public:

    /**
     * @brief Initialize prototypes from a stream of events
     * 
     * @param layer the layer to be initialized
     * @param events the stream of events to be used
     */
    virtual void initializePrototypes(Layer& layer, const Events& events) const;

    /**
     * @brief Initialize prototypes from a vector of streams of events
     * 
     * @param layer the layer to be initialized
     * @param event_streams the vector of streams of events to be used
     */
    virtual void initializePrototypes(Layer& layer, const std::vector<Events>& event_streams) const;

protected:

    /**
     * @brief Actual initialization algorithm
     * 
     * This is the method where the algorithm is performed and must be implemented in subclasses.
     * 
     * @param layer the layer to be initialized
     * @param time_surfaces a list of valid time surfaces that can be used for the initialization
     */
    virtual void initializationAlgorithm(Layer& layer, const std::vector<Eigen::ArrayXXf>& time_surfaces) const = 0;

};


/**
 * @brief Uniformly initialize the layer
 * 
 * This class initialize the prototypes by simply choosing random valid time surfaces,
 * generated from streams of events, with uniform probabilities.
 */
class LayerUniformInitializer : public LayerInitializer {

protected:
    void initializationAlgorithm(Layer& layer, const std::vector<Eigen::ArrayXXf>& time_surfaces) const override;

};


/**
 * @brief k-means++ initialization
 * 
 * This class implements the initialization algorithm of k-means++ to choose the prototypes
 * among the valid time surfaces generated from streams of events.
 */
class LayerPlusPlusInitializer : public LayerInitializer {

protected:
    void initializationAlgorithm(Layer& layer, const std::vector<Eigen::ArrayXXf>& time_surfaces) const override;

};


/**
 * @brief Random initialization
 * 
 * Initialize the prototypes of the layer with random time surfaces, ignoring the input events.
 * This may be useful for debugging purposes.
 */
class LayerRandomInitializer : public LayerInitializer {

public:

    /**
     * @brief Initialize prototypes from a stream of events
     * 
     * In this subclass, events are not used.
     * 
     * @param layer the layer to be initialized
     * @param events not used
     */
    void initializePrototypes(Layer& layer, const Events& events) const override;

    /**
     * @brief Initialize prototypes from a vector of streams of events
     * 
     * In this subclass, events are not used.
     * 
     * @param layer the layer to be initialized
     * @param event_streams not used
     */
    void initializePrototypes(Layer& layer, const std::vector<Events>& event_streams) const override;

protected:

    /**
     * @brief Actual initialization algorithm
     * 
     * This is the method where the algorithm is performed and must be implemented in subclasses.
     * 
     * @param layer the layer to be initialized
     * @param time_surfaces not used
     */
    void initializationAlgorithm(Layer& layer, const std::vector<Eigen::ArrayXXf>& time_surfaces) const override;

};

}

#endif