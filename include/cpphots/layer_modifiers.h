/**
 * @file layer_modifiers.h
 * @brief Components that modify the behaviour of Layers
 */
#ifndef CPPHOTS_LAYER_MODIFIERS_H
#define CPPHOTS_LAYER_MODIFIERS_H

#include <vector>

#include "types.h"
#include "interfaces/layer_modifiers.h"


namespace cpphots {

/**
 * @brief Changes the output of a Layer to ArrayHOTS
 * 
 * If added to a Layer, output events will be emitted as:
 * {t, k, y, 0}
 * where k is the clustering output.
 */
struct ArrayLayer : public interfaces::Clonable<ArrayLayer, interfaces::EventRemapper> {

    event remapEvent(event ev, uint16_t k) override;

    void toStream(std::ostream& out) const override;

    void fromStream(std::istream& in) override;

};

/**
 * @brief Changes the output of a Layer to a single dimension
 * 
 * If added to a Layer, output events will be remapped to a single dimension,
 * as follows:
 * {t, w*h*k + w*y + x, 0, 0}
 * where k is the clustering output and w,h are the dimensions of the context.
 */
class SerializingLayer : public interfaces::Clonable<SerializingLayer, interfaces::EventRemapper> {

public:

    /**
     * @brief Construct a new SerializingLayer modifier
     * 
     * This constructor should not be used explicitly, it is provided only to
     * load this modifier from a stream.
     */
    SerializingLayer() {}

    /**
     * @brief Construct a new SerializingLayer modifier
     * 
     * @param width horizontal size of the context
     * @param height vertical size of the context
     */
    SerializingLayer(uint16_t width, uint16_t height);

    event remapEvent(event ev, uint16_t k) override;

    /**
     * @brief Returns the size of the context
     * 
     * @return {width, height}
     */
    std::pair<uint16_t, uint16_t> getSize() const;

    void toStream(std::ostream& out) const override;

    void fromStream(std::istream& in) override;

private:
    uint16_t w, h;

};


/**
 * @brief Subsample layer into super cells, without averaging
 * 
 * If added to a layer, the output coordinates of the events will be mapped
 * using cells of a fixed size, thus reducing the output dimensionality.
 * Cells may be overlapping, causing the layer to emit more than one event.
 */
class SuperCell : public interfaces::Clonable<SuperCell, interfaces::SuperCell> {

public:

    /**
     * @brief Construct a new SuperCell modifier
     * 
     * This constructor should not be used explicitly, it is provided only to
     * load this modifier from a stream.
     */
    SuperCell() {}

    /**
     * @brief Construct a new SuperCell modifier
     * 
     * @param width width of the context
     * @param height height of the context
     * @param K size of the cells
     */
    SuperCell(uint16_t width, uint16_t height, uint16_t K);

    std::pair<uint16_t, uint16_t> findCell(uint16_t ex, uint16_t ey) const override;

    std::pair<uint16_t, uint16_t> getSize() const override;

    std::pair<uint16_t, uint16_t> getCellSizes() const override;

    /**
     * @brief Average time surfaces over a cell
     * 
     * This function actually returns the time surface unmodified.
     * 
     * @param ts new time surface computed
     * @param cx x coordinate of the cell
     * @param cy y coordinate of the cell
     * @return averaged time surface
     */
    TimeSurfaceType averageTS(const TimeSurfaceType& ts, uint16_t cx, uint16_t cy) override;

    void toStream(std::ostream& out) const override;

    void fromStream(std::istream& in) override;

protected:

    /**
     * @brief Width of the context
     */
    uint16_t width;

    /**
     * @brief Height of the context
     */
    uint16_t height;

    /**
     * @brief Size of the cells
     */
    uint16_t K;

    /**
     * @brief Number of cells (horizontal)
     */
    uint16_t wcell;

    /**
     * @brief Number of cells (vertical)
     */
    uint16_t hcell;

    /**
     * @brief Max horizontal processing size
     */
    uint16_t wmax;

    /**
     * @brief Max vertical processing size
     */
    uint16_t hmax;

    /**
     * @brief Get the center of a cell in event-space
     * 
     * @param cx cell x
     * @param cy cell y
     * @return event-space coordinates of the center
     */
    std::pair<uint16_t, uint16_t> getCellCenter(uint16_t cx, uint16_t cy) const;

    /**
     * @brief Check whether some events coordinates are in a certain cell or not
     * 
     * @param cx cell x
     * @param cy cell y
     * @param ex event x
     * @param ey event y
     * @return true if event is in cell
     * @return false otherwise
     */
    bool isInCell(uint16_t cx, uint16_t cy, uint16_t ex, uint16_t ey) const;

};


/**
 * @brief Average time surfaces over cells
 * 
 * If added to a layer, time surfaces will be averaged over supercells.
 */
class SuperCellAverage : public interfaces::Clonable<SuperCellAverage, SuperCell> {

public:

    /**
     * @brief Construct a new Super Cell Average object
     * 
     * This constructor should not be used explicitly, it is provided only to
     * load this modifier from a stream.
     */
    SuperCellAverage() {}

    /**
     * @brief Construct a new SuperCellAverage modifier
     * 
     * @param width width of the context
     * @param height height of the context
     * @param K size of the cells
     */
    SuperCellAverage(uint16_t width, uint16_t height, uint16_t K);

    /**
     * @copydoc interfaces::SuperCell::averageTS
     */
    TimeSurfaceType averageTS(const TimeSurfaceType& ts, uint16_t cx, uint16_t cy) override;

    void toStream(std::ostream& out) const override;

    void fromStream(std::istream& in) override;

private:

    struct CellMem {
        cpphots::TimeSurfaceType ts;
        unsigned int count;
    };

    std::vector<std::vector<CellMem>> cells;

};

}

#endif