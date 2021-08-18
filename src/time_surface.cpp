#include "cpphots/time_surface.h"

#include "cpphots/load.h"


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

    cpphots_assert(x < width && y < height);

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

void TimeSurfaceBase::toStream(std::ostream& out) const {

    out << width << " ";
    out << height << " ";
    out << Rx << " ";
    out << Ry << " ";
    out << Wx << " ";
    out << Wy << " ";
    out << tau << " ";
    out << min_events << std::endl;

}

void TimeSurfaceBase::fromStream(std::istream& in) {

    in >> width;
    in >> height;
    in >> Rx;
    in >> Ry;
    in >> Wx;
    in >> Wy;
    in >> tau;
    in >> min_events;

    reset();

}


std::pair<TimeSurfaceType, bool> LinearTimeSurface::compute(uint64_t t, uint16_t x, uint16_t y) const {

    cpphots_assert(x < width && y < height);

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

void LinearTimeSurface::toStream(std::ostream& out) const {

    writeMetacommand(out, "LINEARTIMESURFACE");
    TimeSurfaceBase::toStream(out);

}

void LinearTimeSurface::fromStream(std::istream& in) {

    matchMetacommandOptional(in, "LINEARTIMESURFACE");
    TimeSurfaceBase::fromStream(in);

}


WeightedLinearTimeSurface::WeightedLinearTimeSurface() {}

WeightedLinearTimeSurface::WeightedLinearTimeSurface(uint16_t width, uint16_t height, uint16_t Rx, uint16_t Ry, TimeSurfaceScalarType tau, const TimeSurfaceType& weightmatrix)
    :Clonable(width, height, Rx, Ry, tau) {

    if (weightmatrix.rows() != height || weightmatrix.cols() != width) {
        throw std::invalid_argument("Wrong size for time surface weight matrix, should be " + std::to_string(height) + "x" + std::to_string(width));
    }

    setWeightMatrix(weightmatrix);

}

std::pair<TimeSurfaceType, bool> WeightedLinearTimeSurface::compute(uint64_t t, uint16_t x, uint16_t y) const {

    cpphots_assert(x < width && y < height);

    auto [ts, good] = LinearTimeSurface::compute(t, x, y);

    // override for the full context
    if (Rx == 0)
        x = 0;
    if (Ry == 0)
        y = 0;

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

void WeightedLinearTimeSurface::toStream(std::ostream& out) const {

    writeMetacommand(out, "WEIGHTEDLINEARTIMESURFACE");
    TimeSurfaceBase::toStream(out);

    out << weights;

}

void WeightedLinearTimeSurface::fromStream(std::istream& in) {

    matchMetacommandOptional(in, "WEIGHTEDLINEARTIMESURFACE");
    TimeSurfaceBase::fromStream(in);

    weights = TimeSurfaceType::Zero(height+2*Ry, width+2*Rx);

    for (uint16_t y = 0; y < height+2*Ry; y++) {
        for (uint16_t x = 0; x < width+2*Rx; x++) {
            in >> weights(y, x);
        }
    }

}

TimeSurfacePool::~TimeSurfacePool() {
    delete_surfaces();
}

TimeSurfacePool::TimeSurfacePool(const TimeSurfacePool& other) {

    for (auto surf : other.surfaces) {
        surfaces.push_back(surf->clone());
    }

}

TimeSurfacePool::TimeSurfacePool(TimeSurfacePool&& other) {

    surfaces = other.surfaces;
    other.surfaces.clear();

}

TimeSurfacePool& TimeSurfacePool::operator=(const TimeSurfacePool& other) {

    delete_surfaces();
    for (auto surf : other.surfaces) {
        surfaces.push_back(surf->clone());
    }

    return *this;

}

TimeSurfacePool& TimeSurfacePool::operator=(TimeSurfacePool&& other) {

    surfaces = other.surfaces;
    other.surfaces.clear();

    return *this;

}

void TimeSurfacePool::toStream(std::ostream& out) const {

    writeMetacommand(out, "TIMESURFACEPOOL");

    out << surfaces.size() << "\n";
    for (const auto& ts : surfaces) {
        out << *ts;
    }

}

void TimeSurfacePool::fromStream(std::istream& in) {

    delete_surfaces();

    matchMetacommandOptional(in, "TIMESURFACEPOOL");

    surfaces.clear();
    size_t n_surfaces;
    in >> n_surfaces;
    surfaces.resize(n_surfaces);
    for (auto& sur : surfaces) {
        sur = loadTSFromStream(in);
    }

}

void TimeSurfacePool::delete_surfaces() {
    for (size_t i = 0; i < surfaces.size(); i++) {
        delete surfaces[i];
    }
    surfaces.clear();
}

}