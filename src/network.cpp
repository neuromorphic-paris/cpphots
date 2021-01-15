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
        layers.push_back(Layer(width, height, Rx, Ry, tau, polarities, N));
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
        layers.push_back(Layer(width, height, Rx[l], Ry[l], tau[l], polarities, N[l]));
        polarities = N[l];
    }

}

event Network::process(uint64_t t, uint16_t x, uint16_t y, uint16_t p) {

    for (size_t l = 0; l < layers.size(); l++) {
        event ev = layers[l].process(t, x, y, p);
        if (ev == invalid_event)
            return invalid_event;
        t = ev.t;
        x = ev.x;
        y = ev.y;
        p = ev.p;
    }

    return {t, x, y, p};

}

Events Network::process(const Events& events) {

    Events retevents = events;
    for (auto& layer : layers) {
        retevents = layer.process(retevents);
    }
    return retevents;

}

std::vector<Events> Network::process(const std::vector<Events>& event_streams) {

    std::vector<Events> reststreams = event_streams;
    for (auto& layer : layers) {
        reststreams = layer.process(reststreams);
    }
    return reststreams;

}

size_t Network::getNumLayers() const {
    return layers.size();
}

unsigned int Network::getInputPolarities() const {
    return inputPolarities;
}

Layer& Network::getLayer(size_t l) {
    return layers[l];
}

std::vector<uint32_t> Network::getLastHistogram() const {
    return layers[layers.size()-1].getHistogram();
}

void Network::resetLayers() {
    for (auto& l : layers) {
        l.resetSurfaces();
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

std::string Network::getDescription() const {

    std::string ret = "[";

    for (size_t i = 0; i < layers.size(); i++) {
        if (i > 0)
            ret += " ";
        ret += "l" + std::to_string(i) + "=" + layers[i].getDescription();
        if (i < layers.size()-1)
            ret += "\n";
    }

    ret += "]";

    return ret;

}


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
        Layer l;
        in >> l;
        network.layers.push_back(l);
    }

    return in;

}

}