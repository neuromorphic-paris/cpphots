#include "cpphots/network.h"


namespace cpphots {

Network::Network() {}

Network::Network(uint16_t width, uint16_t height, uint16_t polarities,
                 uint16_t num_layers,
                 uint16_t Rx1, uint16_t Ry1, uint16_t K_R,
                 float tau1, float K_tau,
                 uint16_t N1, uint16_t K_N)
    :inputPolarities(polarities) {

    uint16_t Rx = Rx1;
    uint16_t Ry = Ry1;
    float tau = tau1;
    uint16_t N = N1;

    for (uint16_t l = 0; l < num_layers; l++) {
        auto layer = cpphots::create_layer(TimeSurfacePool(width, height, Rx, Ry, tau, polarities),
                                               Clusterer(N));
        layers.push_back(layer);
        Rx *= K_R;
        Ry *= K_R;
        tau *= K_tau;
        polarities = N;
        N *= K_N;
    }

}

Network::Network(uint16_t width, uint16_t height, uint16_t polarities,
                const std::vector<uint16_t>& Rx, const std::vector<uint16_t>& Ry,
                const std::vector<float> tau,
                const std::vector<uint16_t> N)
    :inputPolarities(polarities) {

    // check parameters
    uint16_t num_layers = Rx.size();

    if (num_layers == 0 || num_layers != Ry.size() || num_layers != tau.size() || num_layers != N.size()) {
        throw std::invalid_argument("Sizes of parameters do not match in Network constructor.");
    }

    for (uint16_t l = 0; l < num_layers; l++) {
        auto layer = cpphots::create_layer(TimeSurfacePool(width, height, Rx[l], Ry[l], tau[l], polarities),
                                           Clusterer(N[l]));
        layers.push_back(layer);
        polarities = N[l];
    }

}

Events Network::process(uint64_t t, uint16_t x, uint16_t y, uint16_t p) {

    return process({t, x, y, p});

}

Events Network::process(const event& ev) {

    Events evs{ev};
    for (auto& layer : layers) {
        
        Events next_evs;

        for (auto& nev : evs) {
            Events pevs = layer.process(nev);
            next_evs.insert(next_evs.end(), pevs.begin(), pevs.end());
        }
        
        if (next_evs.empty())
            return next_evs;
        
        evs = next_evs;

    }

    return evs;

}

Events Network::process(const Events& events) {

    Events retevents;

    for (const auto& ev : events) {
        auto evs = process(ev);
        retevents.insert(retevents.begin(), evs.begin(), evs.end());
    }

    return retevents;

}

std::vector<Events> Network::process(const std::vector<Events>& event_streams) {

    std::vector<Events> reststreams;

    for (const auto& stream : event_streams) {
        reststreams.push_back(process(stream));
    }

    return reststreams;

}

size_t Network::getNumLayers() const {
    return layers.size();
}

unsigned int Network::getInputPolarities() const {
    return inputPolarities;
}

LayerType& Network::getLayer(size_t l) {
    return layers[l];
}

std::vector<uint32_t> Network::getLastHistogram() const {
    return layers[layers.size()-1].getHistogram();
}

void Network::resetLayers() {
    for (auto& l : layers) {
        l.reset();
    }
}

void Network::toggleLearningAll(bool enable) {
    for (auto& l : layers) {
        l.toggleLearning(enable);
    }
}

void Network::toggleLearningLayer(size_t l, bool enable) {
    for (size_t i = 0; i < layers.size(); i++) {
        if (i == l)
            layers[i].toggleLearning(enable);
        else
            layers[i].toggleLearning(!enable);
    }
}

// std::string Network::getDescription() const {

//     std::string ret = "[";

//     for (size_t i = 0; i < layers.size(); i++) {
//         if (i > 0)
//             ret += " ";
//         ret += "l" + std::to_string(i) + "=" + layers[i].getDescription();
//         if (i < layers.size()-1)
//             ret += "\n";
//     }

//     ret += "]";

//     return ret;

// }


std::ostream& operator<<(std::ostream& out, const Network& network) {

    out << network.inputPolarities << " ";
    out << network.layers.size() << "\n";

    for (const auto& l : network.layers) {
        out << l;
    }

    return out;

}

std::istream& operator>>(std::istream& in, Network& network) {

    in >> network.inputPolarities;

    size_t n_layers;
    in >> n_layers;
    network.layers.clear();
    for (size_t i = 0; i < n_layers; i++) {
        std::string tmp;
        std::getline(in, tmp);
        LayerType l;
        in >> l;
        network.layers.push_back(l);
    }

    return in;

}

}