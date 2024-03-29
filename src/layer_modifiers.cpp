#include "cpphots/layer_modifiers.h"

#include <iostream>


namespace cpphots {


event ArrayLayer::remapEvent(event ev, uint16_t k) {

    ev.x = k;
    ev.p = 0;

    return ev;

}

void ArrayLayer::toStream(std::ostream& out) const {
    writeMetacommand(out, "ARRAYLAYER");
    out << 0;
}

void ArrayLayer::fromStream(std::istream& in) {
    matchMetacommandOptional(in, "ARRAYLAYER");
    int n;
    in >> n;
}


SerializingLayer::SerializingLayer(uint16_t width, uint16_t height)
    :w(width), h(height) {}

event SerializingLayer::remapEvent(event ev, uint16_t k) {

    int x = (int)w*h*k + w*ev.y + ev.x;

    if (x > std::numeric_limits<uint16_t>::max()) {
        throw std::runtime_error("Remapping exceeds uint16_t precision");
    }

    ev.x = x;
    ev.y = 0;
    ev.p = 0;

    return ev;

}

std::pair<uint16_t, uint16_t> SerializingLayer::getSize() const {
    return {w, h};
}

void SerializingLayer::toStream(std::ostream& out) const {
    writeMetacommand(out, "SERIALIZINGLAYER");
    out << w << " " << h << std::endl;
}

void SerializingLayer::fromStream(std::istream& in) {
    matchMetacommandOptional(in, "SERIALIZINGLAYER");
    in >> w;
    in >> h;
}



SuperCell::SuperCell(uint16_t width, uint16_t height, uint16_t K)
    :width(width), height(height), K(K) {

    // width
    wcell = 1 + (width - K) / K;
    wmax = K + (wcell - 1) * K;
    if (width != wmax) {
        std::cerr << "Width " << width << " is higher than the closest multiple of K. Events with x over " << wmax << " will be ignored" << std::endl;
    }

    // heigth
    hcell = 1 + (height - K) / K;
    hmax = K + (hcell - 1) * K;
    if (height != hmax) {
        std::cerr << "Height " << height << " is higher than the closest multiple of K. Events with y over " << hmax << " will be ignored" << std::endl;
    }

}

std::pair<uint16_t, uint16_t> SuperCell::findCell(uint16_t ex, uint16_t ey) const {

    if (ex >= wmax || ey >= hmax){
        return invalid_coordinates;
    }

    // get cell coordinates
    uint16_t lx = ex / K;
    uint16_t ly = ey / K;

    return {lx, ly};

}

std::pair<uint16_t, uint16_t> SuperCell::getSize() const {
    return {width, height};
}

std::pair<uint16_t, uint16_t> SuperCell::getCellSizes() const {
    return {wcell, hcell};
}

TimeSurfaceType SuperCell::averageTS(const TimeSurfaceType& ts, uint16_t cx, uint16_t cy) {
    return ts;
}

void SuperCell::toStream(std::ostream& out) const {
    writeMetacommand(out, "SUPERCELL");
    out << width << " ";
    out << height << " ";
    out << K << " ";
    out << wcell << " ";
    out << hcell << " ";
    out << wmax << " ";
    out << hmax << std::endl;
}

void SuperCell::fromStream(std::istream& in) {
    matchMetacommandOptional(in, "SUPERCELL");
    in >> width;
    in >> height;
    in >> K;
    in >> wcell;
    in >> hcell;
    in >> wmax;
    in >> hmax;
}

std::pair<uint16_t, uint16_t> SuperCell::getCellCenter(uint16_t cx, uint16_t cy) const {

    return {cx * K + K / 2, cy * K + K / 2};

}

bool SuperCell::isInCell(uint16_t cx, uint16_t cy, uint16_t ex, uint16_t ey) const {

    auto [ncx, ncy] = getCellCenter(cx, cy);
    uint16_t csz = K / 2;
    if ((ncx - csz <= ex && ex <= ncx + csz) && (ncy - csz <= ey && ey <= ncy + csz)) {
        return true;
    }
    
    return false;

}


SuperCellAverage::SuperCellAverage(uint16_t width, uint16_t height, uint16_t K)
    :SuperCell(width, height, K) {

    cells = std::vector<std::vector<CellMem>>(hcell, std::vector<CellMem>(wcell));

}

TimeSurfaceType SuperCellAverage::averageTS(const TimeSurfaceType& ts, uint16_t cx, uint16_t cy) {

    if (cx >= wcell || cy >= hcell) {
        throw std::invalid_argument("Invalid cell index for time surface averaging");
    }

    CellMem& cell = cells[cy][cx];

    if (cell.count == 0) {
        cell.count = 1;
        cell.ts = ts;
        return ts;
    }

    cell.ts += ts;
    cell.count++;

    return cell.ts / cell.count;

}

void SuperCellAverage::toStream(std::ostream& out) const {
    writeMetacommand(out, "SUPERCELLAVERAGE");
    out << width << " ";
    out << height << " ";
    out << K << " ";
    out << wcell << " ";
    out << hcell << " ";
    out << wmax << " ";
    out << hmax << std::endl;
}

void SuperCellAverage::fromStream(std::istream& in) {
    matchMetacommandOptional(in, "SUPERCELLAVERAGE");
    in >> width;
    in >> height;
    in >> K;
    in >> wcell;
    in >> hcell;
    in >> wmax;
    in >> hmax;
    cells = std::vector<std::vector<CellMem>>(hcell, std::vector<CellMem>(wcell));
}

}