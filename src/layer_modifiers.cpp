#include "cpphots/layer_modifiers.h"

#include <iostream>


namespace cpphots {

Averaging::Averaging(uint16_t width, uint16_t height, uint16_t K, uint16_t overlap)
    :K(K), o(overlap) {

    // width
    wcell = 1 + (width - K) / (K - overlap);
    uint16_t wsugg = K + (wcell-1)*(K - overlap);
    if (width != wsugg) {
        std::cerr << "Width " << width << " is not a multiple of K and overlap. Suggested: " << wsugg << std::endl;
    }

    // heigth
    hcell = 1 + (height - K) / (K - overlap);
    uint16_t hsugg = K + (hcell-1)*(K - overlap);
    if (height != hsugg) {
        std::cerr << "Height " << height << " is not a multiple of K and overlap. Suggested: " << hsugg << std::endl;
    }

    cells = std::vector<std::vector<Cell>>(hcell, std::vector<Cell>(wcell));

}

std::vector<std::pair<uint16_t, uint16_t>> Averaging::findCells(uint16_t ex, uint16_t ey) const {

    // get best cell coordinates
    uint16_t lx = (ex >= o ? ex-o : 0) / (K-o);
    uint16_t ly = (ey >= o ? ey-o : 0) / (K-o);
    
    std::vector<std::pair<uint16_t, uint16_t>> cells{{lx, ly}};

    if (o > 0) {
        if ((lx+1 < wcell) && (ly+0 < hcell) && isInCell(lx+1, ly+0, ex, ey))
            cells.push_back({lx+1, ly+0});
        if ((lx+0 < wcell) && (ly+1 < hcell) && isInCell(lx+0, ly+1, ex, ey))
            cells.push_back({lx+0, ly+1});
        if ((lx+1 < wcell) && (ly+1 < hcell) && isInCell(lx+1, ly+1, ex, ey))
            cells.push_back({lx+1, ly+1});
    }

    return cells;

}

TimeSurfaceType Averaging::averageTS(const TimeSurfaceType& ts, uint16_t cx, uint16_t cy) {

    if (cx >= wcell || cy >= hcell) {
        throw std::invalid_argument("Invalid cell index for time surface averaging");
    }

    Cell& cell = cells[cy][cx];

    if (cell.count == 0) {
        cell.count = 1;
        cell.ts = ts;
        return ts;
    }

    cell.ts += ts;
    cell.count++;

    return cell.ts / cell.count;

}

std::pair<uint16_t, uint16_t> Averaging::getCellCenter(uint16_t cx, uint16_t cy) const {

    return {cx * (K - o) + K / 2, cy * (K-o) + K / 2};

}

bool Averaging::isInCell(uint16_t cx, uint16_t cy, uint16_t ex, uint16_t ey) const {

    auto [ncx, ncy] = getCellCenter(cx, cy);
    uint16_t csz = K / 2;
    if ((ncx - csz <= ex && ex <= ncx + csz) && (ncy - csz <= ey && ey <= ncy + csz)) {
        return true;
    }
    
    return false;

}

}