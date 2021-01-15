#include "cpphots/layer.h"

#include <algorithm>
#include <random>
#include <set>
#include <ctime>


namespace cpphots {

Layer::Layer() {}

Layer::Layer(uint16_t width, uint16_t height,
             uint16_t Rx,uint16_t Ry, float tau,
             uint16_t polarities, uint16_t features)
    :TimeSurfacePool(width, height, Rx, Ry, tau, polarities), Clusterer(features) {

    // layer description
    description = "[";
    description += "Rx=" + std::to_string(Rx);
    description += ", Ry=" + std::to_string(Ry);
    description += ", tau=" + std::to_string(tau);
    description += ", pol=" + std::to_string(polarities);
    description += ", N=" + std::to_string(features);
    description += "]";

}

event Layer::process(uint64_t t, uint16_t x, uint16_t y, uint16_t p, bool skip_check) {

    if (!isInitialized()) {
        throw std::runtime_error("Cannot process event: Layer is not initialized.");
    }

    // compute the current time surface
    auto [surface, good] = updateAndCompute(t, x, y, p);

    // if the surface is not good we say it upstream
    if (!skip_check && !good) {
        return invalid_event;
    }

    // find closest kernel
    uint16_t k = cluster(surface);

    // generate new event
    return event{t, x, y, k};

}

Events Layer::process(const Events& events, bool skip_check) {

    resetSurfaces();

    Events retevents;
    for (const auto& ev : events) {
        auto nev = process(ev, skip_check);
        if (nev != invalid_event)
            retevents.push_back(nev);
    }
    return retevents;

}

std::vector<Events> Layer::process(const std::vector<Events>& event_streams, bool skip_check) {

    std::vector<Events> retstreams;
    for (const auto& events : event_streams) {
        auto nevs = process(events, skip_check);
        retstreams.push_back(nevs);
    }
    return retstreams;

}

void Layer::resetSurfaces() {

    TimeSurfacePool::reset();

    Clusterer::reset();

}

std::string Layer::getDescription() const{

    return description;
}


std::ostream& operator<<(std::ostream& out, const Layer& layer) {

    out << layer.description << "\n";

    // time surfaces
    out << static_cast<const TimeSurfacePool&>(layer);

    // prototypes
    out << static_cast<const Clusterer&>(layer);

    return out;

}

std::istream& operator>>(std::istream& in, Layer& layer) {

    std::getline(in, layer.description);

    // surfaces
    in >> static_cast<TimeSurfacePool&>(layer);

    // clustering
    in >> static_cast<Clusterer&>(layer);

    return in;

}


void layerInitializePrototypes(const ClustererInitializerType& initializer, Layer& layer, const Events& events, bool valid_only) {

    // store all time surfaces
    layer.resetSurfaces();
    std::vector<TimeSurfaceType> time_surfaces;
    for (auto& ev : events) {
        auto surface_good = layer.updateAndCompute(ev);
        if (valid_only && surface_good.second) {
            time_surfaces.push_back(surface_good.first);
        } else if (!valid_only) {
            time_surfaces.push_back(surface_good.first);
        }
    }

    if (time_surfaces.size() < layer.getNumClusters()) {
        throw std::runtime_error("Not enough good events to initialize prototypes.");
    }

    initializer(layer, time_surfaces);

}

void layerInitializePrototypes(const ClustererInitializerType& initializer, Layer& layer, const std::vector<Events>& event_streams, bool valid_only) {

    // store all time surfaces
    std::vector<TimeSurfaceType> time_surfaces;
    for (auto& stream : event_streams) {
        layer.resetSurfaces();
        for (auto& ev : stream) {
            auto surface_good = layer.updateAndCompute(ev);
            if (valid_only && surface_good.second) {
                time_surfaces.push_back(surface_good.first);
            } else if (!valid_only) {
                time_surfaces.push_back(surface_good.first);
            }
        }
    }

    if (time_surfaces.size() < layer.getNumClusters()) {
        throw std::runtime_error("Not enough good events to initialize prototypes.");
    }

    initializer(layer, time_surfaces);

}

}