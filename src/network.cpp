#include "cpphots/network.h"

#include "cpphots/load.h"


namespace cpphots {

Network::Network() {}

void Network::addLayer(LayerPtr layer) {
    layers.push_back(layer);
}

Events Network::process(uint64_t t, uint16_t x, uint16_t y, uint16_t p, bool skip_check) {

    return process({t, x, y, p}, skip_check);

}

Events Network::process(const event& ev, bool skip_check) {

    Events evs{ev};
    for (auto& layer : layers) {

        Events next_evs;

        for (auto& nev : evs) {
            Events pevs = layer->process(nev, skip_check);
            next_evs.insert(next_evs.end(), pevs.begin(), pevs.end());
        }

        if (next_evs.empty())
            return next_evs;

        evs = next_evs;

    }

    return evs;

}

size_t Network::getNumLayers() const {
    return layers.size();
}

LayerBase& Network::operator[](size_t pos) {
    return *layers[pos];
}

const LayerBase& Network::operator[](size_t pos) const {
    return *layers[pos];
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
        l->reset();
    }
}


void Network::toStream(std::ostream& out) const {
    writeMetacommand(out, "NETWORKBEGIN");
    for (const auto& l : layers) {
        l->toStream(out);
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
            // addLayer(loadLayerFromStream(in));
        }

        cmd = Streamable::getNextMetacommand(in);

    }

}

}