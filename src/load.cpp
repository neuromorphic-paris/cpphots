#include "cpphots/load.h"

#include <stdexcept>

#include "cpphots/interfaces/streamable.h"
#include "cpphots/time_surface.h"
#include "cpphots/clustering/cosine.h"
#ifdef CPPHOTS_WITH_PEREGRINE
#include "cpphots/clustering/gmm.h"
#endif
#include "cpphots/layer_modifiers.h"


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

    throw std::runtime_error("Unknow time surface type " + metacmd);

}

interfaces::TimeSurfacePoolCalculator* loadTSPoolFromStream(std::istream& in) {

    auto metacmd = interfaces::Streamable::getNextMetacommand(in);

    if (metacmd == "TIMESURFACEPOOL") {
        TimeSurfacePool* pool = new TimeSurfacePool();
        pool->fromStream(in);
        return pool;
    }

    throw std::runtime_error("Unkown time surface pool type " + metacmd);

}

interfaces::Clusterer* loadClustererFromStream(std::istream& in) {

    auto metacmd = interfaces::Streamable::getNextMetacommand(in);

    if (metacmd == "COSINECLUSTERER") {
        CosineClusterer* clust = new CosineClusterer();
        clust->fromStream(in);
        return clust;
    }

#ifdef CPPHOTS_WITH_PEREGRINE
    if (metacmd == "GMMCLUSTERER") {
        GMMClusterer* clust = new GMMClusterer();
        clust->fromStream(in);
        return clust;
    }
#endif

    throw std::runtime_error("Unkown clusterer type " + metacmd);

}

interfaces::EventRemapper* loadRemapperFromStream(std::istream& in) {

    auto metacmd = interfaces::Streamable::getNextMetacommand(in);

    if (metacmd == "ARRAYLAYER") {
        ArrayLayer* remapper = new ArrayLayer();
        remapper->fromStream(in);
        return remapper;
    }

    if (metacmd == "SERIALIZINGLAYER") {
        SerializingLayer* remapper = new SerializingLayer();
        remapper->fromStream(in);
        return remapper;
    }

    throw std::runtime_error("Unkown event remapper type " + metacmd);
}

interfaces::SuperCell* loadSuperCellFromStream(std::istream& in) {

    auto metacmd = interfaces::Streamable::getNextMetacommand(in);

    if (metacmd == "SUPERCELL") {
        SuperCell* supercell = new SuperCell();
        supercell->fromStream(in);
        return supercell;
    }

    if (metacmd == "SUPERCELLAVERAGE") {
        SuperCellAverage* supercell = new SuperCellAverage();
        supercell->fromStream(in);
        return supercell;
    }

    throw std::runtime_error("Unkown super cell type " + metacmd);

}

}