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
    :TimeSurfacePool(width, height, Rx, Ry, tau, polarities), features(features) {

    // layer description
    description = "[";
    description += "Rx=" + std::to_string(Rx);
    description += ", Ry=" + std::to_string(Ry);
    description += ", tau=" + std::to_string(tau);
    description += ", pol=" + std::to_string(polarities);
    description += ", N=" + std::to_string(features);
    description += "]";

    hist.resize(features);

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
    uint16_t k = -1;
    float mindist = std::numeric_limits<float>::max();
    for (uint i = 0; i < prototypes.size(); i++) {
        float d = (surface - prototypes[i]).matrix().norm();
        if (d < mindist) {
            mindist = d;
            k = i;
        }
    }

    // update histogram
    hist[k]++;

    if (learning) {

        // increase kernel activation
        prototypes_activations[k]++;

        // beta
        float beta = prototypes[k].cwiseProduct(surface).sum() / prototypes[k].matrix().norm() / surface.matrix().norm();
        // float beta = maxcosine;

        // alpha
        float alpha = 1. / (1. + prototypes_activations[k]);

        // update kernel
        prototypes[k] += alpha * beta * (surface - prototypes[k]);

    }

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

uint16_t Layer::getFeatures() const {
    return features;
}

std::vector<uint32_t> Layer::getHist() const {
    return hist;
}

std::vector<TimeSurfaceType> Layer::getPrototypes() const {
    return prototypes;
}

void Layer::resetSurfaces() {

    TimeSurfacePool::reset();

    hist.clear();
    hist.resize(features);

}

bool Layer::toggleLearning(bool enable) {
    bool prev = learning;
    learning = enable;
    return prev;
}

std::string Layer::getDescription() const{

    return description;
}

void Layer::clearPrototypes() {
    prototypes.clear();
    prototypes_activations.clear();
}

void Layer::addPrototype(const TimeSurfaceType& proto) {
    if (isInitialized()) {
        throw std::runtime_error("Trying to add a prototype to an already initialized Layer.");
    }
    prototypes.push_back(proto);
    prototypes_activations.push_back(0);
}


std::ostream& operator<<(std::ostream& out, const Layer& layer) {

    out << layer.description << "\n";
    out << layer.features << " ";
    out << layer.learning << " ";

    // time surfaces
    out << static_cast<const TimeSurfacePool&>(layer);

    // prototypes
    out << layer.prototypes.size() << " ";
    for (const auto& pa : layer.prototypes_activations) {
        out << pa << " ";
    }
    out << "\n";
    for (const auto& p : layer.prototypes) {
        out << p << "\n";
    }

    return out;

}

std::istream& operator>>(std::istream& in, Layer& layer) {

    std::getline(in, layer.description);
    in >> layer.features;
    in >> layer.learning;

    // surfaces
    in >> static_cast<TimeSurfacePool&>(layer);

    // prototypes
    uint16_t wx = layer.getSurface(0).getWx();
    uint16_t wy = layer.getSurface(0).getWy();
    size_t n_prototypes;
    in >> n_prototypes;
    layer.prototypes_activations.clear();
    layer.prototypes_activations.resize(n_prototypes);
    for (auto& pa : layer.prototypes_activations) {
        in >> pa;
    }
    layer.prototypes.clear();
    for (size_t i = 0; i < n_prototypes; i++) {
        TimeSurfaceType p = TimeSurfaceType::Zero(wy, wx);
        for (uint16_t y = 0; y < wy; y++) {
            for (uint16_t x = 0; x < wx; x++) {
                in >> p(y, x);
            }
        }
        layer.prototypes.push_back(p);
    }

    layer.hist.clear();
    layer.hist.resize(layer.features);

    return in;

}


void LayerInitializer::initializePrototypes(Layer& layer, const Events& events, bool valid_only) const {

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

    if (time_surfaces.size() < layer.getFeatures()) {
        throw std::runtime_error("Not enough good events to initialize prototypes.");
    }

    initializationAlgorithm(layer, time_surfaces);

}

void LayerInitializer::initializePrototypes(Layer& layer, const std::vector<Events>& event_streams, bool valid_only) const {

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

    if (time_surfaces.size() < layer.getFeatures()) {
        throw std::runtime_error("Not enough good events to initialize prototypes.");
    }

    initializationAlgorithm(layer, time_surfaces);

}


void LayerUniformInitializer::initializationAlgorithm(Layer& layer, const std::vector<TimeSurfaceType>& time_surfaces) const {

    std::vector<TimeSurfaceType> selected;
    std::sample(time_surfaces.begin(), time_surfaces.end(), std::back_inserter(selected), layer.getFeatures(), std::mt19937{std::random_device{}()});

    for (auto& p : selected) {
        layer.addPrototype(p);
    }

}


void LayerPlusPlusInitializer::initializationAlgorithm(Layer& layer, const std::vector<TimeSurfaceType>& time_surfaces) const {

    // chosen surfaces
    std::set<int> chosen;

    // choose first time surface at random
    std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<> idist(0, time_surfaces.size());
    int first = idist(gen);
    layer.addPrototype(time_surfaces[first]);
    chosen.insert(first);

    std::vector<float> distances(time_surfaces.size());
    float distsum = 0.0;

    for (size_t k = 1; k < layer.getFeatures(); k++) {

        distsum = 0.0;

        // compute all squared distances
        for (size_t ts = 0; ts < time_surfaces.size(); ts++) {

            float mindist = std::numeric_limits<float>::max();
            for (const auto& p : layer.getPrototypes()) {
                float d = (p - time_surfaces[ts]).matrix().norm();
                d = d * d;
                if (d < mindist)
                    mindist = d;
            }

            distances[ts] = mindist;
            distsum += mindist;

        }

        // choose random surface, based on distances
        std::uniform_real_distribution<float> rdist(0.0, distsum);
        float x = rdist(gen);
        float currdist = 0.0;

        for (size_t ts = 0; ts < time_surfaces.size(); ts++) {
            if (x < currdist + distances[ts]) {
                layer.addPrototype(time_surfaces[ts]);
                chosen.insert(ts);
                break;
            }
            currdist += distances[ts];
        }

    }

    if (chosen.size() != layer.getFeatures()) {
        throw std::runtime_error("Something went wrong with the plusplus initialization.");
    }

}


void LayerRandomInitializer::initializePrototypes(Layer& layer, const Events& events, bool valid_only) const {
    initializationAlgorithm(layer, {});
}

void LayerRandomInitializer::initializePrototypes(Layer& layer, const std::vector<Events>& event_streams, bool valid_only) const {
    initializationAlgorithm(layer, {});
}

void LayerRandomInitializer::initializationAlgorithm(Layer& layer, const std::vector<TimeSurfaceType>& time_surfaces) const {

    uint16_t Wx = layer.getSurface(0).getWx();
    uint16_t Wy = layer.getSurface(0).getWy();

    std::srand((unsigned int) std::time(0));

    for (uint16_t i = 0; i < layer.getFeatures(); i++) {
        layer.addPrototype(TimeSurfaceType::Random(Wy, Wx) + 1.f /2.f);
    }

}

}