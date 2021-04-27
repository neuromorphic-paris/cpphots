/**
 * @file layer_traits.h
 * @brief Helper functions to check traits of Layer components
 */
#ifndef CPPHOTS_LAYER_TRAITS_H
#define CPPHOTS_LAYER_TRAITS_H

#include <type_traits>
#include <ostream>
#include <istream>


namespace cpphots {

/**
 * @brief Check if a type is resettable
 * 
 * A type is resettable if has a reset() method.
 * 
 * @tparam T type to check
 */
template<typename T,
         typename = void>
struct is_resettable : std::false_type {};

/**
 * @brief Check if a type is resettable
 * 
 * A type is resettable if has a reset() method.
 * 
 * @tparam T type to check
 */
template<typename T>
struct is_resettable <
    T,
    std::enable_if_t<std::is_member_function_pointer_v<decltype(&T::reset)>> > {
    /**
     * @brief Value of the expression
     */
    static constexpr bool value = std::is_member_function_pointer_v<decltype(&T::reset)>;
};

/**
 * @brief Helper variable template of is_resettable
 * 
 * @tparam T type to check
 */
template<typename T>
inline constexpr bool is_resettable_v = is_resettable<T>::value;


/**
 * @brief Check if a type has a size
 * 
 * A type has a size if has a getSize() method.
 * 
 * @tparam T type to check
 */
template<typename T,
         typename = void>
struct has_size : std::false_type {};

/**
 * @brief Check if a type has a size
 * 
 * A type has a size if has a getSize() method.
 * 
 * @tparam T type to check
 */
template<typename T>
struct has_size <
    T,
    std::enable_if_t<std::is_member_function_pointer_v<decltype(&T::getSize)>> > {
    /**
     * @brief Value of the expression
     */
    static constexpr bool value = std::is_member_function_pointer_v<decltype(&T::getSize)>;
};

/**
 * @brief Helper variable template of has_size
 * 
 * @tparam T type to check
 */
template<typename T>
inline constexpr bool has_size_v = has_size<T>::value;

}

#endif