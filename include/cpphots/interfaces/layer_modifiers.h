/**
 * @file interfaces/layer_modifiers.h
 * @brief Interfaces for classe that can modify the behaviour of a layer
 */
#ifndef CPPHOTS_INTERFACES_LAYER_MODIFIERS_H
#define CPPHOTS_INTERFACES_LAYER_MODIFIERS_H

#include "../types.h"
#include "streamable.h"
#include "clonable.h"


namespace cpphots{

namespace interfaces {

/**
 * @brief Interface for a modifier that remaps events
 * 
 * A remapper usually changes the coordinates or the polarity of an event,
 * withoud modifying the event timestamp.
 */
struct EventRemapper : public virtual Streamable, public ClonableBase<EventRemapper> {

    /**
     * @brief Destroy the EventRemapper object
     */
    ~EventRemapper() {}

    /**
     * @brief Remap event
     * 
     * @param ev event
     * @param k cluster id
     * @return remapped event
     */
    virtual event remapEvent(event ev, uint16_t k) = 0;

};


/**
 * @brief Interface for a modifier that subsample the layer into super cells
 * 
 * Usually such modifiers, changes the output coordinates of the events
 * using cells, thus reducing the output dimensionality.
 * Cells may be overlapping, causing the layer to emit more than one event.
 * 
 * Modifiers can also average time surfaces from the same cell.
 */
class SuperCell : public virtual Streamable, public ClonableBase<SuperCell> {

public:

    /**
     * @brief Destroy the SuperCell object
     */
    ~SuperCell() {}

    /**
     * @brief Find coordinates of cells that are over certain coordinates
     * 
     * The output contains more than one set of coordinates only if overlap > 0.
     * 
     * @param ex x coordinate of the event
     * @param ey y coordinate of the event
     * @return vector of cell coordinates
     */
    virtual std::vector<std::pair<uint16_t, uint16_t>> findCells(uint16_t ex, uint16_t ey) const = 0;

    /**
     * @brief Returns the size of the context
     * 
     * @return {width, height}
     */
    virtual std::pair<uint16_t, uint16_t> getSize() const = 0;

    /**
     * @brief Get the number of the horizontal and vertical cells
     * 
     * @return {horizontal cells, vertical cells}
     */
    virtual std::pair<uint16_t, uint16_t> getCellSizes() const = 0;

    /**
     * @brief Average time surfaces over a cell
     * 
     * @param ts new time surface computed
     * @param cx x coordinate of the cell
     * @param cy y coordinate of the cell
     * @return averaged time surface
     */
    virtual TimeSurfaceType averageTS(const TimeSurfaceType& ts, uint16_t cx, uint16_t cy) = 0;

};

}

}

#endif