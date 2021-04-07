#include "cpphots/time_surface.h"


namespace cpphots {

TimeSurfaceBase::TimeSurfaceBase() {}

TimeSurfaceBase::TimeSurfaceBase(uint16_t width, uint16_t height, uint16_t Rx, uint16_t Ry, TimeSurfaceScalarType tau)
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

void TimeSurfaceBase::update(uint64_t t, uint16_t x, uint16_t y) {

    context(y+Ry, x+Rx) = t;

}

std::pair<uint16_t, uint16_t> TimeSurfaceBase::getSize() const {
    return {width, height};
}

void TimeSurfaceBase::reset() {
    context = TimeSurfaceType::Zero(height+2*Ry, width+2*Rx) - tau;  // makes sense, but is not in the paper
}

TimeSurfaceType TimeSurfaceBase::getContext() const {
    return context.block(Ry, Rx, height, width);
}

std::ostream& operator<<(std::ostream& out, const TimeSurfaceBase& ts) {

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

std::istream& operator>>(std::istream& in, TimeSurfaceBase& ts) {

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


std::pair<TimeSurfaceType, bool> LinearTimeSurface::compute(uint64_t t, uint16_t x, uint16_t y) const {

    // override for the full context
    if (Rx == 0)
        x = 0;
    if (Ry == 0)
        y = 0;

    TimeSurfaceType retmat = context.block(y, x, Wy, Wx);  // should be (x-Rx, y-Ry), but the context is padded

    TimeSurfaceType ret = 1. - (t - retmat) / tau;

    bool good = (ret > 0.).count() >= min_events;

    return std::make_pair((ret <= 0.).select(0., ret), good);

}

std::pair<TimeSurfaceType, bool> LinearTimeSurface::compute(const event& ev) const {
    return compute(ev.t, ev.x, ev.y);
}

TimeSurfaceType LinearTimeSurface::sampleContext(uint64_t t) const {

    TimeSurfaceType ret = 1. - (t - getContext()) / tau;

    return (ret <= 0.).select(0., ret);

}

std::ostream& operator<<(std::ostream& out, const LinearTimeSurface& ts) {

    out << static_cast<const TimeSurfaceBase&>(ts);

    return out;

}

std::istream& operator>>(std::istream& in, LinearTimeSurface& ts) {

    in >> static_cast<TimeSurfaceBase&>(ts);

    return in;

}


WeightedLinearTimeSurface::WeightedLinearTimeSurface() {}

WeightedLinearTimeSurface::WeightedLinearTimeSurface(uint16_t width, uint16_t height, uint16_t Rx, uint16_t Ry, TimeSurfaceScalarType tau, const TimeSurfaceType& weightmatrix)
    :LinearTimeSurface(width, height, Rx, Ry, tau) {

    if (weightmatrix.rows() != height || weightmatrix.cols() != width) {
        throw std::invalid_argument("Wrong size for time surface weight matrix, should be " + std::to_string(height) + "x" + std::to_string(width));
    }

    setWeightMatrix(weightmatrix);

}

std::pair<TimeSurfaceType, bool> WeightedLinearTimeSurface::compute(uint64_t t, uint16_t x, uint16_t y) const {

    auto [ts, good] = LinearTimeSurface::compute(t, x, y);

    TimeSurfaceType w = weights.block(y, x, Wy, Wx);

    ts = ts * w;

    return {ts, good};

}

std::pair<TimeSurfaceType, bool> WeightedLinearTimeSurface::compute(const event& ev) const {
    return compute(ev.t, ev.x, ev.y);
}

TimeSurfaceType WeightedLinearTimeSurface::sampleContext(uint64_t t) const {

    TimeSurfaceType ts = LinearTimeSurface::sampleContext(t);

    TimeSurfaceType w = weights.block(Ry, Rx, height, width);

    return ts * w;

}

void WeightedLinearTimeSurface::setWeightMatrix(const TimeSurfaceType& weightmatrix) {

    weights = TimeSurfaceType::Zero(height+2*Ry, width+2*Rx);

    weights.block(Ry, Rx, height, width) = weightmatrix;

}

std::ostream& operator<<(std::ostream& out, const WeightedLinearTimeSurface& ts) {

    out << static_cast<const LinearTimeSurface&>(ts) << std::endl;

    out << ts.weights;

    return out;

}

std::istream& operator>>(std::istream& in, WeightedLinearTimeSurface& ts) {

    in >> static_cast<LinearTimeSurface&>(ts);

    ts.weights = TimeSurfaceType::Zero(ts.height+2*ts.Ry, ts.width+2*ts.Rx);

    for (uint16_t y = 0; y < ts.height+2*ts.Ry; y++) {
        for (uint16_t x = 0; x < ts.width+2*ts.Rx; x++) {
            in >> ts.weights(y, x);
        }
    }

    return in;

}

}