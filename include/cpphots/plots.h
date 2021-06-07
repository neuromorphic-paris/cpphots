/**
 * @file plots.h
 * @brief Time surface plotting utilities
 */
#ifndef CPPHOTS_PLOTS_H
#define CPPHOTS_PLOTS_H

#include "types.h"


namespace cpphots {

/**
 * @brief Plot a time surface
 * 
 * This functions accepts both one and two-dimensional time surfaces.
 * 
 * @param ts the timesurface to be plotted
 */
void plot_ts(const TimeSurfaceType& ts);

/**
 * @brief Plot multiple time surfaces into a single figure
 * 
 * This functions accepts both one and two-dimensional time surfaces.
 * The arrangement of plots, in terms of rows and columns, can be optionally specified.
 * If no arrangment is specified, a best-effort arrangement that tries to minimize the number of empty subslots is performed.
 * 
 * @param mts the time surfaces to be plotted
 * @param rows the number of rows for the arrangement
 * @param cols the number of columns for the arrangement
 * @param colorcode whether a different color should be used for each time surface or not
 */
void plot_multiple_ts(const std::vector<TimeSurfaceType>& mts, unsigned int rows = 0, unsigned int cols = 0, bool colorcode = true);

}

#endif