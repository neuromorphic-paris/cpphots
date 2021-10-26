#include <cpphots/types.h>

#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace pybind11::literals;


PYBIND11_MODULE(cpphots, m) {

    m.doc() = "A HOTS library";

    py::class_<cpphots::event>(m, "event")
        .def(py::init<uint64_t, uint16_t, uint16_t, uint16_t>(), "t"_a = 0, "x"_a = 0, "y"_a = 0, "p"_a = 0)
        .def_readwrite("t", &cpphots::event::t)
        .def_readwrite("x", &cpphots::event::x)
        .def_readwrite("y", &cpphots::event::y)
        .def_readwrite("p", &cpphots::event::p)
        .def("__repr__", [] (const cpphots::event& ev) {
            std::string ret;
            ret += "(";
            ret += std::to_string(ev.t) + ", ";
            ret += std::to_string(ev.x) + ", ";
            ret += std::to_string(ev.y) + ", ";
            ret += std::to_string(ev.p) + ")";
            return ret;});

    m.attr("invalid_event") = cpphots::invalid_event;

}