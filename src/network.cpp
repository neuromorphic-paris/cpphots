#include "cpphots/network.h"

#include "cpphots/load.h"


namespace cpphots {

Network::Network() {}

void Network::createLayer(interfaces::TimeSurfacePoolCalculator* tspool,
                          interfaces::Clusterer* clusterer,
                          interfaces::EventRemapper* remapper,
                          interfaces::SuperCell* supercell) {
    layers.emplace_back(tspool, clusterer, remapper, supercell);
}

void Network::addLayer(const Layer& layer) {
    layers.push_back(layer);
}

event Network::process(uint64_t t, uint16_t x, uint16_t y, uint16_t p, bool skip_check) {

    return process({t, x, y, p}, skip_check);

}

event Network::process(const event& ev, bool skip_check) {

    event nev = ev;
    for (auto& layer : layers) {

        // Events next_evs;
        nev = layer.process(nev, skip_check);

        if (nev == invalid_event) {
            return invalid_event;
        }

    }

    return nev;

}

size_t Network::getNumLayers() const {
    return layers.size();
}

Layer& Network::getLayer(size_t pos) {
        return layers[pos];
    }

const Layer& Network::getLayer(size_t pos) const {
    return layers[pos];
}

Layer& Network::operator[](size_t pos) {
    return layers[pos];
}

const Layer& Network::operator[](size_t pos) const {
    return layers[pos];
}

Layer& Network::back() {
    return layers.back();
}

const Layer& Network::back() const {
    return layers.back();
}

Network Network::getSubnetwork(int start, int stop) const {

    Network ret;
    if (start < 0) {
        start = layers.size() + start;
    }

    if (stop <= 0) {
        stop = layers.size() + stop;
    }

    for (int i = start; i < stop; i++) {
        ret.layers.push_back(layers[i]);
    }

    return ret;

}

void Network::reset() {
    for (auto& l : layers) {
        l.reset();
    }
}


void Network::toStream(std::ostream& out) const {
    writeMetacommand(out, "NETWORKBEGIN");
    for (const auto& l : layers) {
        l.toStream(out);
        out << std::endl;
    }
    writeMetacommand(out, "NETWORKEND");
}

void Network::fromStream(std::istream& in) {

    layers.clear();

    matchMetacommandRequired(in, "NETWORKBEGIN");

    auto cmd = Streamable::getNextMetacommand(in);

    while (cmd != "NETWORKEND") {

        if (cmd == "LAYERBEGIN") {
            Layer layer;
            layer.fromStream(in);
            layers.push_back(layer);
        }

        cmd = Streamable::getNextMetacommand(in);

    }

}

Network::iterator Network::begin() noexcept {
    return layers.begin();
}

Network::const_iterator Network::begin() const noexcept {
    return layers.begin();
}

Network::const_iterator Network::cbegin() const noexcept {
    return layers.cbegin();
}

Network::iterator Network::end() noexcept {
    return layers.end();
}

Network::const_iterator Network::end() const noexcept {
    return layers.end();
}

Network::const_iterator Network::cend() const noexcept {
    return layers.cend();
}


Network operator+(const Network& n1, const Network& n2) {

    Network ret = n1;

    for (auto& l : n2) {
        ret.addLayer(l);
    }

    return ret;

}


}