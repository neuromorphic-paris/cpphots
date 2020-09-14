/**
 * @file plots.h
 * @brief Time surface plotting utilities
 */

#include "time_surface.h"


namespace cpphots {

/**
 * @brief Plot a time surface
 * 
 * @param ts the timesurface to be plotted
 */
void plot_ts(const TimeSurfaceType& ts);

/**
 * @brief Plot multiple time surfaces into a single figure
 * 
 * @param mts the time surfaces to be plotted
 */
void plot_multiple_ts(const std::vector<TimeSurfaceType>& mts);

}
