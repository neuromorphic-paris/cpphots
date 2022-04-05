#include "cpphots/dynamic_time_surface.h"

#include <iostream>

namespace cpphots {

DynamicTimeSurface::DynamicTimeSurface(uint16_t width, uint16_t height, uint16_t Rx, uint16_t Ry, TimeSurfaceScalarType initial_m)
    : TimeSurfaceBase(width, height, Rx, Ry, 0.0), initial_m(initial_m), m(initial_m) {}

void DynamicTimeSurface::update(uint64_t t, uint16_t x, uint16_t y) {
    TimeSurfaceBase::update(t, x, y);
    TimeSurfaceScalarType d = 1.0 / (TimeSurfaceScalarType(t - last_timestamp) * m + 1.0);
    m = d * m + 1.0e-6;
    last_timestamp = t;
}

std::pair<TimeSurfaceType, bool> DynamicTimeSurface::compute(uint64_t t, uint16_t x, uint16_t y) const {
    cpphots_assert(x < width && y < height);

    // override for the full context
    if (Rx == 0)
        x = 0;
    if (Ry == 0)
        y = 0;

    TimeSurfaceType retmat = context.block(y, x, Wy, Wx);  // should be (x-Rx, y-Ry), but the context is padded

    bool good = (retmat > 0.).count() >= min_events;

    TimeSurfaceType ret = 1. / ((t - retmat) * m + 1.0);

    return std::make_pair((retmat <= 0.).select(0., ret), good);
}

std::pair<TimeSurfaceType, bool> DynamicTimeSurface::compute(const event& ev) const {
    return compute(ev.t, ev.x, ev.y);
}

TimeSurfaceType DynamicTimeSurface::sampleContext(uint64_t t) const {
    return 1. / ((t - getContext()) * m + 1.0);
}

void DynamicTimeSurface::reset() {
    TimeSurfaceBase::reset();
    last_timestamp = 0;
    m = initial_m;
}

void DynamicTimeSurface::toStream(std::ostream& out) const {
    writeMetacommand(out, "DYNAMICTIMESURFACE");
    TimeSurfaceBase::toStream(out);
    out << last_timestamp << " ";
    out << initial_m << " ";
    out << m << std::endl;
}

void DynamicTimeSurface::fromStream(std::istream& in) {
    matchMetacommandOptional(in, "DYNAMICTIMESURFACE");
    TimeSurfaceBase::fromStream(in);
    in >> last_timestamp;
    in >> initial_m;
    in >> m;
}

}