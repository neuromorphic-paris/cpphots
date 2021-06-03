#include "cpphots/load.h"

#include <stdexcept>

#include "cpphots/time_surface.h"
#include "cpphots/interfaces/streamable.h"

#ifdef CPPHOTS_WITH_PEREGRINE
#include "cpphots/clustering_gmm.h"
#endif


namespace cpphots {

TimeSurfacePtr loadTSFromStream(std::istream& in) {

    auto metacmd = interfaces::Streamable::getNextMetacommand(in);

    if (metacmd == "LINEARTIMESURFACE") {
        LinearTimeSurface* ts = new LinearTimeSurface();
        ts->fromStream(in);
        return TimeSurfacePtr(ts);
    }

    if (metacmd == "WEIGHTEDLINEARTIMESURFACE") {
        WeightedLinearTimeSurface* ts = new WeightedLinearTimeSurface();
        ts->fromStream(in);
        return TimeSurfacePtr(ts);
    }

    throw std::runtime_error("Unknow time surface type");

}

}