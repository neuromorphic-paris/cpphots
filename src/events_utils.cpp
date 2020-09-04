#include "events_utils.h"

#include <sepia.hpp>


namespace cpphots {

Events loadFromFile(const std::string& filename, const std::unordered_map<bool, uint16_t>& change_polarities) {

    Events ret;

    sepia::join_observable<sepia::type::dvs>(sepia::filename_to_ifstream(filename),
                                             [&](sepia::dvs_event dvs_event) { ret.push_back({dvs_event.t,
                                                                                              dvs_event.x,
                                                                                              dvs_event.y,
                                                                                              change_polarities.at(dvs_event.is_increase)});} );

    return ret;

}

}