#include "time_surface.h"


namespace cpphots {

TimeSurface::TimeSurface() {}

TimeSurface::TimeSurface(uint16_t width, uint16_t height, uint16_t Rx, uint16_t Ry, float tau)
    :width(width), height(height), Rx(Rx), Ry(Ry), tau(tau) {
    
    reset();

    // compute window size
    Wx = 2*Rx+1;
    Wy = 2*Ry+1;
    min_events = 2*std::sqrt(Rx*Ry);  // same as 2R if Rx == Ry

    if (Rx == 0) {
        Wx = width;
        min_events = std::numeric_limits<uint16_t>::max();  // force the 25% in this case
    }

    if (Ry == 0) {
        Wy = height;
        min_events = std::numeric_limits<uint16_t>::max();  // force the 25% in this case
    }

    min_events = std::min(min_events, static_cast<uint16_t>(0.25*(Wx)*(Wy)));  // maximum number of minimum events should not exceed 25% (maximum of 2R/(2R+1)^2)
    if (min_events == 0) {  // there should be at least one event...
        min_events = 1;
    }

}

void TimeSurface::update(uint16_t x, uint16_t y, uint64_t t) {
    
    context(y+Ry, x+Rx) = t;

}

std::pair<Eigen::ArrayXXf, bool> TimeSurface::compute(uint16_t x, uint16_t y, uint64_t t) const {
    
    // override for the full context
    if (Rx == 0)
        x = 0;
    if (Ry == 0)
        y = 0;

    Eigen::ArrayXXf retmat = context.block(y, x, Wy, Wx);  // should be (x-Rx, y-Ry), but the context is padded

    auto ret = 1. - (t - retmat) / tau;

    bool good = (ret > 0.).count() >= min_events;

    return std::make_pair((ret <= 0.).select(0., ret), good);

}

void TimeSurface::reset() {
    context = Eigen::ArrayXXf::Zero(height+2*Ry, width+2*Rx) - tau;  // makes sense, but is not in the paper
}

}