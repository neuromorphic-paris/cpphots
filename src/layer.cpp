#include "cpphots/layer.h"

// TODO: remove these
#include "cpphots/time_surface.h"
#include "cpphots/clustering_cosine.h"
#include "cpphots/layer_modifiers.h"
#include "cpphots/clustering_gmm.h"


namespace cpphots {

Layer::Layer(interfaces::TimeSurfacePoolCalculator* tspool,
             interfaces::Clusterer* clusterer,
             interfaces::EventRemapper* remapper,
             interfaces::SuperCell* supercell)
          :tspool(tspool), clusterer(clusterer), remapper(remapper), supercell(supercell) {}

void Layer::addTSPool(interfaces::TimeSurfacePoolCalculator* tspool) {
    delete this->tspool;
    this->tspool = tspool;
}

void Layer::addClusterer(interfaces::Clusterer* clusterer) {
    delete this->clusterer;
    this->clusterer = clusterer;
}

void Layer::addRemapper(interfaces::EventRemapper* remapper) {
    delete this->remapper;
    this->remapper = remapper;
}

void Layer::addSuperCell(interfaces::SuperCell* supercell) {
    delete this->supercell;
    this->supercell = supercell;
}

Events Layer::process(uint64_t t, uint16_t x, uint16_t y, uint16_t p, bool skip_check) {

    if (!tspool) {
        throw std::runtime_error("Pool must be set in order to process events");
    }

    auto [surface, good] = tspool->updateAndCompute(t, x, y, p);

    // if the surface is not good we say it upstream
    if (!skip_check && !good) {
        return Events{};
    }

    std::vector<TimeSurfaceType> surfaces;
    std::vector<std::pair<uint16_t, uint16_t>> coords;

    // check averaging
    if (supercell) {
        coords = supercell->findCells(x, y);
        for (auto [cx, cy] : coords) {
            surfaces.push_back(supercell->averageTS(surface, cx, cy));
        }
    } else {
        surfaces.push_back(surface);
        coords.push_back({x, y});
    }

    // if there is a clustering algorithm we can use it
    if (clusterer) {

        Events retevents;

        for (size_t i = 0; i < surfaces.size(); i++) {

            auto& surface = surfaces[i];
            auto [x, y] = coords[i];

            auto k = clusterer->cluster(surface);

            if (remapper) {
                retevents.push_back(remapper->remapEvent(event{t, x, y, p}, k));
            } else {
                retevents.push_back(event{t, x, y, k});
            }

        }

        return retevents;
    }

    Events retevents;
    for (auto [ex, ey] : coords) {
        retevents.push_back(event{t, ex, ey, p});
    }

    return retevents;

}

bool Layer::canCluster() const {
    return clusterer != nullptr;
}

void Layer::reset() {
    tspool->reset();
    clusterer->reset();
}

void Layer::toStream(std::ostream& out) const {

    writeMetacommand(out, "LAYERBEGIN");

    if (tspool) {
        writeMetacommand(out, "POOL");
        out << *tspool << std::endl;
    } else {
        writeMetacommand(out, "SKIP");
    }

    if (clusterer) {
        writeMetacommand(out, "CLUST");
        out << *clusterer << std::endl;
    } else {
        writeMetacommand(out, "SKIP");
    }

    if (remapper) {
        writeMetacommand(out, "REMAPPER");
        out << *remapper << std::endl;
    } else {
        writeMetacommand(out, "SKIP");
    }

    if (supercell) {
        writeMetacommand(out, "SUPERCELL");
        out << *supercell << std::endl;
    } else {
        writeMetacommand(out, "SKIP");
    }

    writeMetacommand(out, "LAYEREND");

}

void Layer::fromStream(std::istream& in) {

    matchMetacommandOptional(in, "LAYERBEGIN");

    auto cmd = getNextMetacommand(in);
    if (cmd == "POOL") {
        // TODO: change these
        delete tspool;
        tspool = new TimeSurfacePool();
        tspool->fromStream(in);
    }

    cmd = getNextMetacommand(in);
    if (cmd == "CLUST") {
        cmd = getNextMetacommand(in);
        delete clusterer;
        if (cmd == "COSINECLUSTERER") {
            clusterer = new CosineClusterer();
        } else {
            clusterer = new GMMClusterer();
        }
        clusterer->fromStream(in);
    }

    cmd = getNextMetacommand(in);
    if (cmd == "REMAPPER") {
        cmd = getNextMetacommand(in);
        delete remapper;
        if (cmd == "ARRAYLAYER") {
            remapper = new ArrayLayer();
        } else {
            remapper = new SerializingLayer();
        }
        remapper->fromStream(in);
    }

    cmd = getNextMetacommand(in);
    if (cmd == "SUPERCELL") {
        cmd = getNextMetacommand(in);
        delete supercell;
        if (cmd == "SUPERCELL") {
            supercell = new SuperCell();
        } else {
            supercell = new SuperCellAverage();
        }
        supercell->fromStream(in);
    }

    matchMetacommandOptional(in, "LAYEREND");

}


void layerInitializePrototypes(const ClustererInitializerType& initializer, Layer& layer, const Events& events, bool valid_only) {

    // store all time surfaces
    layer.reset();
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
        layer.reset();
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