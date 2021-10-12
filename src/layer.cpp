#include "cpphots/layer.h"

#include "cpphots/load.h"


namespace cpphots {

Layer::Layer(interfaces::TimeSurfacePoolCalculator* tspool,
             interfaces::Clusterer* clusterer,
             interfaces::EventRemapper* remapper,
             interfaces::SuperCell* supercell)
          :tspool(tspool), clusterer(clusterer), remapper(remapper), supercell(supercell) {}

Layer::~Layer() {
    delete_components();
}

Layer::Layer(const Layer& other) {

    if (other.tspool) {
        tspool = other.tspool->clone();
    } else {
        tspool = nullptr;
    }

    if (other.clusterer) {
        clusterer = other.clusterer->clone();
    } else {
        clusterer = nullptr;
    }

    if (other.remapper) {
        remapper = other.remapper->clone();
    } else {
        remapper = nullptr;
    }

    if (other.supercell) {
        supercell = other.supercell->clone();
    } else {
        supercell = nullptr;
    }

}

Layer::Layer(Layer&& other) {

    tspool = other.tspool;
    other.tspool = nullptr;

    clusterer = other.clusterer;
    other.clusterer = nullptr;

    remapper = other.remapper;
    other.remapper = nullptr;

    supercell = other.supercell;
    other.supercell = nullptr;

}

Layer& Layer::operator=(const Layer& other) {

    delete_components();

    if (other.tspool) {
        tspool = other.tspool->clone();
    } else {
        tspool = nullptr;
    }

    if (other.clusterer) {
        clusterer = other.clusterer->clone();
    } else {
        clusterer = nullptr;
    }

    if (other.remapper) {
        remapper = other.remapper->clone();
    } else {
        remapper = nullptr;
    }

    if (other.supercell) {
        supercell = other.supercell->clone();
    } else {
        supercell = nullptr;
    }

    return *this;

}

Layer& Layer::operator=(Layer&& other) {

    delete_components();

    tspool = other.tspool;
    other.tspool = nullptr;

    clusterer = other.clusterer;
    other.clusterer = nullptr;

    remapper = other.remapper;
    other.remapper = nullptr;

    supercell = other.supercell;
    other.supercell = nullptr;

    return *this;

}

void Layer::addTSPool(interfaces::TimeSurfacePoolCalculator* tspool) {
    delete this->tspool;
    this->tspool = tspool;
}

interfaces::TimeSurfacePoolCalculator& Layer::getTSPool() {

    if (tspool) {
        return *tspool;
    }

    throw std::runtime_error("No time surface pool present in layer");

}

const interfaces::TimeSurfacePoolCalculator& Layer::getTSPool() const {
    
    if (tspool) {
        return *tspool;
    }

    throw std::runtime_error("No time surface pool present in layer");

}

void Layer::addClusterer(interfaces::Clusterer* clusterer) {
    delete this->clusterer;
    this->clusterer = clusterer;
}

interfaces::Clusterer& Layer::getClusterer() {

    if (clusterer) {
        return *clusterer;
    }

    throw std::runtime_error("No clusterer present in layer");

}

const interfaces::Clusterer& Layer::getClusterer() const {

    if (clusterer) {
        return *clusterer;
    }

    throw std::runtime_error("No clusterer present in layer");

}

void Layer::addRemapper(interfaces::EventRemapper* remapper) {
    delete this->remapper;
    this->remapper = remapper;
}

interfaces::EventRemapper& Layer::getRemapper() {

    if (remapper) {
        return *remapper;
    }

    throw std::runtime_error("No event remapper present in layer");

}

const interfaces::EventRemapper& Layer::getRemapper() const {

    if (remapper) {
        return *remapper;
    }

    throw std::runtime_error("No event remapper present in layer");

}

void Layer::addSuperCell(interfaces::SuperCell* supercell) {
    delete this->supercell;
    this->supercell = supercell;
}

interfaces::SuperCell& Layer::getSuperCell() {

    if (supercell) {
        return *supercell;
    }

    throw std::runtime_error("No supercell present in layer");

}

const interfaces::SuperCell& Layer::getSuperCell() const {

    if (supercell) {
        return *supercell;
    }

    throw std::runtime_error("No supercell present in layer");

}

event Layer::process(uint64_t t, uint16_t x, uint16_t y, uint16_t p, bool skip_check) {

    cpphots_assert(tspool != nullptr);

    auto [surface, good] = tspool->updateAndCompute(t, x, y, p);

    // if the surface is not good we say it upstream
    if (!skip_check && !good) {
        return invalid_event;
    }

    // supercell modifier
    if (supercell) {
        std::tie(x, y) = supercell->findCell(x, y);
        surface = supercell->averageTS(surface, x, y);
        if (x == invalid_coordinates.first || y == invalid_coordinates.second) {
            return invalid_event;
        }
    }

    uint16_t k = p;

    // if there is a clustering algorithm we can use it
    if (clusterer) {
        k = clusterer->cluster(surface);
    }

    // remap event
    if (remapper) {
        return {remapper->remapEvent(event{t, x, y, p}, k)};
    }

    return {t, x, y, k};  // default behaviour

}

bool Layer::canCluster() const {
    return clusterer != nullptr;
}

void Layer::reset() {
    tspool->reset();
    if (clusterer)
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

    delete_components();

    auto cmd = getNextMetacommand(in);

    if (cmd == "LAYERBEGIN") {
        cmd = getNextMetacommand(in);
    }

    if (cmd == "POOL") {
        tspool = loadTSPoolFromStream(in);
    }

    cmd = getNextMetacommand(in);
    if (cmd == "CLUST") {
        clusterer = loadClustererFromStream(in);
    }

    cmd = getNextMetacommand(in);
    if (cmd == "REMAPPER") {
        remapper = loadRemapperFromStream(in);
    }

    cmd = getNextMetacommand(in);
    if (cmd == "SUPERCELL") {
        supercell = loadSuperCellFromStream(in);
    }

    matchMetacommandOptional(in, "LAYEREND");

}

Layer* Layer::clone() const {
    return new Layer(*this);
}

void Layer::delete_components() {

    delete tspool;
    tspool = nullptr;

    delete clusterer;
    clusterer = nullptr;

    delete remapper;
    remapper = nullptr;

    delete supercell;
    supercell = nullptr;

}


void layerSeedCentroids(const ClustererSeedingType& seeding, Layer& layer, const Events& events, bool valid_only) {

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
        throw std::runtime_error("Not enough good events to seed centroids.");
    }

    seeding(layer, time_surfaces);

}

void layerSeedCentroids(const ClustererSeedingType& seeding, Layer& layer, const std::vector<Events>& event_streams, bool valid_only) {

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
        throw std::runtime_error("Not enough good events to seed centroids.");
    }

    seeding(layer, time_surfaces);

}

}