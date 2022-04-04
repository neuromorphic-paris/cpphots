#ifndef CPPHOTS_DYNAMIC_TIME_SURFACE_H
#define CPPHOTS_DYNAMIC_TIME_SURFACE_H

#include "time_surface.h"


namespace cpphots {

class DynamicTimeSurface : public interfaces::Clonable<DynamicTimeSurface, TimeSurfaceBase> {

public:
    DynamicTimeSurface(uint16_t width, uint16_t height, uint16_t Rx, uint16_t Ry, TimeSurfaceScalarType initial_m = 0.0);

    void update(uint64_t t, uint16_t x, uint16_t y) override;

    std::pair<TimeSurfaceType, bool> compute(uint64_t t, uint16_t x, uint16_t y) const override;

    std::pair<TimeSurfaceType, bool> compute(const event& ev) const override;

    TimeSurfaceType sampleContext(uint64_t t) const override;

    void reset() override;

    void toStream(std::ostream& out) const override;

    void fromStream(std::istream& in) override;

private:
    uint64_t last_timestamp = 0;
    cpphots::TimeSurfaceScalarType m;
    cpphots::TimeSurfaceScalarType initial_m;

};

}

#endif  // CPPHOTS_DYNAMIC_TIME_SURFACE_H