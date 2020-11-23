#include "cpphots/events_utils.h"

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


std::ostream& operator<<(std::ostream& out, const event& event) {
    out << "(";
    out << event.t << ", ";
    out << event.x << ", ";
    out << event.y << ", ";
    out << event.p;
    out << ")";
    return out;
}


bool operator==(const event& ev1, const event& ev2) {

    bool ret = true;

    ret = ret && (ev1.t == ev2.t);
    ret = ret && (ev1.x == ev2.x);
    ret = ret && (ev1.y == ev2.y);
    ret = ret && (ev1.p == ev2.p);

    return ret;

}

}