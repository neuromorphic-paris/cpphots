#include "time_surface.h"


namespace cpphots {

TimeSurface::TimeSurface() {}

TimeSurface::TimeSurface(uint16_t width, uint16_t height, uint16_t Rx, uint16_t Ry, float tau)
    :width(width), height(height), Rx(Rx), Ry(Ry), tau(tau) {
    
    reset();

}

void TimeSurface::update(uint16_t x, uint16_t y, uint64_t t) {
    
    context(y+Ry, x+Rx) = t;

}

std::pair<Eigen::ArrayXXf, bool> TimeSurface::compute(uint16_t x, uint16_t y, uint64_t t) const {
    
    Eigen::ArrayXXf retmat = context.block(y, x, 2*Ry+1, 2*Rx+1);  // should be (x-Rx, y-Ry), but the context is padded

    auto ret = 1. - (t - retmat) / tau;

    bool good = (ret > 0.).count() >= Rx+Ry;  // same as 2R if Rx == Ry

    return std::make_pair((ret <= 0.).select(0., ret), good);

}

void TimeSurface::reset() {
    context = Eigen::ArrayXXf::Zero(height+2*Ry, width+2*Rx) - tau;  // makes sense, but is not in the paper
}

}