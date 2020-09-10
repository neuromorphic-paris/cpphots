#include "cpphots/layer.h"

#include <algorithm>
#include <random>
#include <set>
#include <ctime>


namespace cpphots {

Layer::Layer(uint16_t width, uint16_t height,
             uint16_t Rx,uint16_t Ry, float tau,
             uint16_t polarities, uint16_t features)
    :features(features) {

    for (uint16_t i = 0; i < polarities; i++) {
        surfaces.push_back(TimeSurface(width, height, Rx, Ry, tau));
    }

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

std::pair<event, bool> Layer::process(uint16_t x, uint16_t y, uint64_t t, uint16_t p, bool skip_check) {

    if (!isInitialized()) {
        throw std::runtime_error("Cannot process event: Layer is not initialized.");
    }

    if (p >= surfaces.size()) {
        throw std::invalid_argument("Received event with polarity " + std::to_string(p) + " but max polarity is " + std::to_string(surfaces.size()-1));
    }

    // compute the current time surface
    auto surface_good = surfaces[p].updateAndCompute(x, y, t);
    
    // if the surface is not good we say it upstream
    if (!skip_check && !surface_good.second) {
        return std::make_pair(event{t, x, y, p}, false);
    }

    Eigen::ArrayXXf surface = surface_good.first;

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
    return std::make_pair(event{t, x, y, k}, true);

}

Events Layer::process(const Events& events, bool skip_check) {

    resetSurfaces();

    Events retevents;
    for (const auto& ev : events) {
        auto nev = process(ev, skip_check);
        if (nev.second)
            retevents.push_back(nev.first);
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

std::vector<Eigen::ArrayXXf> Layer::getPrototypes() const {
    return prototypes;
}


void Layer::resetSurfaces() {

    for (auto& ts : surfaces) {
        ts.reset();
    }

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

void Layer::addPrototype(const Eigen::ArrayXXf& proto) {
    if (isInitialized()) {
        throw std::runtime_error("Trying to add a prototype to an already initialized Layer.");
    }
    prototypes.push_back(proto);
    prototypes_activations.push_back(0);
}


void LayerInitializer::initializePrototypes(Layer& layer, const Events& events, bool valid_only) const {

    // store all time surfaces
    layer.resetSurfaces();
    std::vector<Eigen::ArrayXXf> time_surfaces;
    for (unsigned int i = 0; i < events.size(); i++) {
        auto surface_good = layer.getSurface(events[i].p).updateAndCompute(events[i]);
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
    std::vector<Eigen::ArrayXXf> time_surfaces;
    for (unsigned int st = 0; st < event_streams.size(); st++) {
        layer.resetSurfaces();
        for (unsigned int i = 0; i < event_streams[st].size(); i++) {
            auto surface_good = layer.getSurface(event_streams[st][i].p).updateAndCompute(event_streams[st][i]);
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


void LayerUniformInitializer::initializationAlgorithm(Layer& layer, const std::vector<Eigen::ArrayXXf>& time_surfaces) const {

    std::vector<Eigen::ArrayXXf> selected;
    std::sample(time_surfaces.begin(), time_surfaces.end(), std::back_inserter(selected), layer.getFeatures(), std::mt19937{std::random_device{}()});

    for (auto& p : selected) {
        layer.addPrototype(p);
    }

}


void LayerPlusPlusInitializer::initializationAlgorithm(Layer& layer, const std::vector<Eigen::ArrayXXf>& time_surfaces) const {

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

    for (unsigned int k = 1; k < layer.getFeatures(); k++) {

        distsum = 0.0;
        
        // compute all squared distances
        for (unsigned int ts = 0; ts < time_surfaces.size(); ts++) {
            
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

        for (unsigned int ts = 0; ts < time_surfaces.size(); ts++) {
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

void LayerRandomInitializer::initializationAlgorithm(Layer& layer, const std::vector<Eigen::ArrayXXf>& time_surfaces) const {

    uint16_t Wx = layer.getSurface(0).getWx();
    uint16_t Wy = layer.getSurface(0).getWy();

    std::srand((unsigned int) std::time(0));

    for (uint16_t i = 0; i < layer.getFeatures(); i++) {
        layer.addPrototype(Eigen::ArrayXXf::Random(Wy, Wy) + 1.f /2.f);
    }

}

}
