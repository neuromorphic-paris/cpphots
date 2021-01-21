/**
 * @file layer_modifiers.h
 * @brief Components that modify the behaviour of Layers
 */


/**
 * @brief Changes the output of a Layer to ArrayHOTS
 * 
 * If added to a Layer, output events will be emitted as:
 * {t, k, y, 0}
 * where k is the clustering output.
 */
struct ArrayLayer {};