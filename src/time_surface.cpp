#include "cpphots/time_surface.h"


namespace cpphots {

TimeSurface::TimeSurface() {}

TimeSurface::TimeSurface(uint16_t width, uint16_t height, uint16_t Rx, uint16_t Ry, TimeSurfaceScalarType tau)
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

void TimeSurface::update(uint64_t t, uint16_t x, uint16_t y) {

    context(y+Ry, x+Rx) = t;

}

std::pair<TimeSurfaceType, bool> TimeSurface::compute(uint64_t t, uint16_t x, uint16_t y) const {

    // override for the full context
    if (Rx == 0)
        x = 0;
    if (Ry == 0)
        y = 0;

    TimeSurfaceType retmat = context.block(y, x, Wy, Wx);  // should be (x-Rx, y-Ry), but the context is padded

    auto ret = 1. - (t - retmat) / tau;

    bool good = (ret > 0.).count() >= min_events;

    return std::make_pair((ret <= 0.).select(0., ret), good);

}

TimeSurfaceType TimeSurface::sampleFullContext(uint64_t t) const {

    auto ret = 1. - (t - context) / tau;

    return (ret <= 0.).select(0., ret);

}

void TimeSurface::reset() {
    context = TimeSurfaceType::Zero(height+2*Ry, width+2*Rx) - tau;  // makes sense, but is not in the paper
}


std::ostream& operator<<(std::ostream& out, const TimeSurface& ts) {

    out << ts.width << " ";
    out << ts.height << " ";
    out << ts.Rx << " ";
    out << ts.Ry << " ";
    out << ts.Wx << " ";
    out << ts.Wy << " ";
    out << ts.tau << " ";
    out << ts.min_events;

    return out;

}

std::istream& operator>>(std::istream& in, TimeSurface& ts) {

    in >> ts.width;
    in >> ts.height;
    in >> ts.Rx;
    in >> ts.Ry;
    in >> ts.Wx;
    in >> ts.Wy;
    in >> ts.tau;
    in >> ts.min_events;

    ts.reset();

    return in;

}


TimeSurfacePool::TimeSurfacePool() {}

TimeSurfacePool::TimeSurfacePool(uint16_t width, uint16_t height, uint16_t Rx, uint16_t Ry, TimeSurfaceScalarType tau, uint16_t polarities) {

    for (uint16_t i = 0; i < polarities; i++) {
        surfaces.push_back(TimeSurface(width, height, Rx, Ry, tau));
    }

}

void TimeSurfacePool::update(uint64_t t, uint16_t x, uint16_t y, uint16_t p) {
    assert_polarity(p);
    surfaces[p].update(t, x, y);
}

std::pair<TimeSurfaceType, bool> TimeSurfacePool::compute(uint64_t t, uint16_t x, uint16_t y, uint16_t p) const {
    assert_polarity(p);
    return surfaces[p].compute(t, x, y);
}

void TimeSurfacePool::reset() {
    for (auto& ts : surfaces) {
        ts.reset();
    }
}

std::vector<TimeSurfaceType> TimeSurfacePool::sampleFullContexts(uint64_t t) {
    std::vector<TimeSurfaceType> ret;
    for (const auto& ts : surfaces) {
        ret.push_back(ts.sampleFullContext(t));
    }
    return ret;
}

void TimeSurfacePool::assert_polarity(uint16_t p) const {
    if (p >= surfaces.size()) {
        throw std::invalid_argument("Polarity index exceeded: " + std::to_string(p) + ". Layer has only " + std::to_string(surfaces.size()) + " input polarities.");
    }
}

std::ostream& operator<<(std::ostream& out, const TimeSurfacePool& pool) {

    out << pool.surfaces.size() << "\n";
    for (const auto& ts : pool.surfaces) {
        out << ts << "\n";
    }

    return out;

}

std::istream& operator>>(std::istream& in, TimeSurfacePool& pool) {

    pool.surfaces.clear();
    size_t n_surfaces;
    in >> n_surfaces;
    pool.surfaces.resize(n_surfaces);
    for (auto& sur : pool.surfaces) {
        in >> sur;
    }

    return in;

}

}
