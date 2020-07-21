#include "events_utils.h"

#include <sepia.hpp>


namespace cpphots {

Events loadFromFile(const std::string& filename, bool merge_polarities) {

    Events ret;

    sepia::join_observable<sepia::type::dvs>(sepia::filename_to_ifstream(filename),
                                             [&](sepia::dvs_event dvs_event) { uint16_t p = dvs_event.is_increase ? 1 : 0;
                                                                               if (merge_polarities) p = 0;
                                                                               ret.push_back({dvs_event.t, dvs_event.x, dvs_event.y, p}); });

    return ret;

}

}