/**
 * @file layer.h
 * @brief Implementation of HOTS layers and of layer initialization procedures
 */

#ifndef LAYER_H
#define LAYER_H

#include <cstdint>
#include <vector>
#include <string>
#include <istream>
#include <ostream>

#include "time_surface.h"
#include "clustering.h"
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
class Layer : public TimeSurfacePool, public Clusterer {

public:

    /**
     * @brief Construct a new Layer object
     * 
     * This constructor should never be used explicitly,
     * it is provided only to create containers with Layer instances.
     */
    Layer();

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
     * If the input event does not produce a valid time surface, then the output event is also not valid,
     * unless validity checks are skipped.
     * 
     * The closest prototype is choosen based on L2 distance.
     * 
     * If learning is enabled, this function will also update the matching prototype.
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @param p polarity of the event
     * @param skip_check if true consider all events as valid
     * @return the new emitted event, possibly invalid
     */
    virtual event process(uint64_t t, uint16_t x, uint16_t y, uint16_t p, bool skip_check = false);

    /**
     * @brief Process an event
     * 
     * Process an event and emit a new event, depending on the closest prototype.
     * If the input event does not produce a valid time surface, then the output event is also not valid,
     * unless validity checks are skipped.
     * 
     * The closest prototype is choosen based on L2 distance.
     * 
     * If learning is enabled, this function will also update the matching prototype.
     * 
     * @param ev the event
     * @param skip_check if true consider all events as valid
     * @return the new emitted event, possibly invalid
     */
    inline event process(const event& ev, bool skip_check = false) {
        return process(ev.t, ev.x, ev.y, ev.p, skip_check);
    }

    /**
     * @brief Process a stream of events
     * 
     * Process a stream of events and emit a new stream of events.
     * Only valid events are emitted, if otherwise specified.
     * 
     * This method resets the time surfaces before processing the events.
     *
     * If learning is enabled, this function will also update matching prototypes.
     * 
     * @param events a stream of events
     * @param skip_check if true consider all events as valid
     * @return a new stream of valid events
     */
    Events process(const Events& events, bool skip_check = false);

    /**
     * @brief Process a vector of stream of events
     * 
     * Process each stream of events and emit a new stream of events for every one of the input streams.
     * Only valid events are emitted, if otherwise specified.
     * 
     * This method resets the time surfaces before processing every stream of events.
     *
     * If learning is enabled, this function will also update matching prototypes.
     * 
     * @param event_streams a vector of stream of events
     * @param skip_check if true consider all events as valid
     * @return a new vector of streams of valid events
     */
    std::vector<Events> process(const std::vector<Events>& event_streams, bool skip_check = false);

    /**
     * @brief Reset the time surfaces
     * 
     * This method resets the time surfaces and the histogram of activations.
     * It should be called before every stream of events.
     */
    virtual void resetSurfaces();

    /**
     * @brief Get layer description
     * 
     * @return a string describing the parameters of the Layer
     */
    std::string getDescription() const;

    /**
     * @brief Stream insertion operator for Layer
     * 
     * Insert parameters of the layer and prototypes on the stream.
     * 
     * @param out output stream
     * @param layer Layer to insert
     * @return output stream
     */
    friend std::ostream& operator<<(std::ostream& out, const Layer& layer);

    /**
     * @brief Stream extraction operator for Layer
     * 
     * Reads parameters and prototypes for the Layer. Previous parameters are overwritten.
     * 
     * @param in input stream
     * @param layer Layer where to extract into
     * @return input stream
     */
    friend std::istream& operator>>(std::istream& in, Layer& layer);

private:
    std::string description;

};


/**
 * @brief Initialize prototypes from a stream of events
 * 
 * @param initializer the initialization algorithm
 * @param layer the layer to be initialized
 * @param events the stream of events to be used
 * @param valid_only use only valid time surfaces for the initialization
 */
void layerInitializePrototypes(const ClustererInitializerType& initializer, Layer& layer, const Events& events, bool valid_only = true);

/**
 * @brief Initialize prototypes from a vector of streams of events
 * 
 * @param initializer the initialization algorithm
 * @param layer the layer to be initialized
 * @param event_streams the vector of streams of events to be used
 * @param valid_only use only valid time surfaces for the initialization
 */
void layerInitializePrototypes(const ClustererInitializerType& initializer, Layer& layer, const std::vector<Events>& event_streams, bool valid_only = true);

}

#endif