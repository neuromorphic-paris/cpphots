#include "cpphots/load.h"

#include "cpphots/interfaces.h"


namespace cpphots {

TimeSurfacePtr loadTSFromStream(std::istream& in) {

    auto metacmd = Streamable::getNextMetacommand(in);

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