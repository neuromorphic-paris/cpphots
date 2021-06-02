#include "cpphots/load.h"

#include <stdexcept>

#include "cpphots/interfaces/streamable.h"

#ifdef CPPHOTS_WITH_PEREGRINE
#include "cpphots/clustering_gmm.h"
#endif


namespace cpphots {

TimeSurfacePtr loadTSFromStream(std::istream& in) {

    auto metacmd = interfaces::Streamable::getNextMetacommand(in);

    if (metacmd == "LINEARTIMESURFACE") {
        LinearTimeSurface* ts = new LinearTimeSurface();
        ts->fromStream(in);
        return TimeSurfacePtr(ts);
    }

    if (metacmd == "WEIGHTEDLINEARTIMESURFACE") {
        WeightedLinearTimeSurface* ts = new WeightedLinearTimeSurface();
        ts->fromStream(in);
        return TimeSurfacePtr(ts);
    }

    throw std::runtime_error("Unknow time surface type");

}


// template <class T0, class... Ts>
// constexpr bool is_one_of = std::disjunction_v<std::is_base_of<std::decay_t<T0>, std::decay_t<Ts>>...>;

// template <typename... T>
// LayerPtr loadLayerFromStream(std::istream& in, const std::tuple<T...>& components) {

//     if (!in.good()) {
//         throw std::runtime_error("An error occurred when loadin a layer from the input stream");
//     }

//     auto cmd = interfaces::Streamable::getNextMetacommand(in);

//     if (cmd == "LAYERBEGIN") {
//         return loadLayerFromStream(in, components);
//     }

//     if (cmd == "LAYEREND") {
//         if constexpr (is_one_of<TimeSurfacePool, T...>) {
//             return std::apply([](auto &&... args) { return create_layer_ptr(args...); }, components);
//         } else {
//             throw std::runtime_error("Layer has no time surface processing component");
//         }
//     }

//     // time surfaces
//     if (cmd == "TIMESURFACEPOOL") {

//         if constexpr (is_one_of<TimeSurfacePool, T...>) {
//             throw std::runtime_error("Component TimeSurfacePool is present twice");
//         } else {
//             TimeSurfacePool comp;
//             comp.fromStream(in);
//             return loadLayerFromStream(in, std::tuple_cat(components, std::tie(comp)));
//         }

//     }

//     // clustering
//     if (cmd == "HOTSCLUSTERER") {

//         if constexpr (is_one_of<Clusterer, T...>) {
//             throw std::runtime_error("Component Clusterer is present twice");
//         } else {
//             HOTSClusterer comp;
//             comp.fromStream(in);
//             return loadLayerFromStream(in, std::tuple_cat(components, std::tie(comp)));
//         }

//     }

//     #ifdef CPPHOTS_WITH_PEREGRINE
//     if (cmd == "GMMCLUSTERER") {

//         if constexpr (is_one_of<Clusterer, T...>) {
//             throw std::runtime_error("Component Clusterer is present twice");
//         } else {
//             GMMClusterer comp;
//             comp.fromStream(in);
//             return loadLayerFromStream(in, std::tuple_cat(components, std::tie(comp)));
//         }

//     }
//     #endif

//     // modifiers
//     if (cmd == "ARRAYLAYER") {

//         if constexpr (is_one_of<EventRemapper, T...>) {
//             throw std::runtime_error("Component EventRemapper is present twice");
//         } else {
//             ArrayLayer comp;
//             comp.fromStream(in);
//             return loadLayerFromStream(in, std::tuple_cat(components, std::tie(comp)));
//         }

//     }

//     if (cmd == "SERIALIZINGLAYER") {

//         if constexpr (is_one_of<EventRemapper, T...>) {
//             throw std::runtime_error("Component EventRemapper is present twice");
//         } else {
//             SerializingLayer comp;
//             comp.fromStream(in);
//             return loadLayerFromStream(in, std::tuple_cat(components, std::tie(comp)));
//         }

//     }

//     if (cmd == "SUPERCELL") {

//         if constexpr (is_one_of<SuperCell, T...>) {
//             throw std::runtime_error("Component SuperCell is present twice");
//         } else {
//             SuperCell comp;
//             comp.fromStream(in);
//             return loadLayerFromStream(in, std::tuple_cat(components, std::tie(comp)));
//         }

//     }

//     if (cmd == "SUPERCELLAVERAGE") {

//         if constexpr (is_one_of<SuperCell, T...>) {
//             throw std::runtime_error("Component SuperCell is present twice");
//         } else {
//             SuperCellAverage comp;
//             comp.fromStream(in);
//             return loadLayerFromStream(in, std::tuple_cat(components, std::tie(comp)));
//         }

//     }

//     throw std::runtime_error("Uknown command '" + cmd + "' encountered while creating layer");

// }

// LayerPtr loadLayerFromStream(std::istream& in) {
//     return loadLayerFromStream(in, std::make_tuple());
// }

}