#include "cpphots/types.h"

namespace cpphots {

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