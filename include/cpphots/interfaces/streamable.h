/**
 * @file interfaces/streamable.h
 * @brief Common interfaces for components that can interact with streams
 */
#ifndef CPPHOTS_INTERFACES_STREAMABLE_H
#define CPPHOTS_INTERFACES_STREAMABLE_H

#include <ostream>
#include <istream>
#include <string>


namespace cpphots {

namespace interfaces {

/**
 * @brief Class that can be inserted to/read from a stream
 * 
 * This interface should be implemented by all components which support
 * saving/loading from file.
 * 
 * This class also provided static functions to handle metacommands, that are
 * string prefixed by '!' which are used to load components for layers and networks.
 */
class Streamable {

public:

    /**
     * @brief Insert to stream
     * 
     * @param out stream where to insert to
     */
    virtual void toStream(std::ostream& out) const = 0;

    /**
     * @brief Extract from stream
     * 
     * Parameters/current state is expected to be overwritten or invalidated.
     * 
     * @param in stream where to extract from
     */
    virtual void fromStream(std::istream& in) = 0;

    /**
     * @brief Get the next metacommand in the stream
     * 
     * @param in input stream
     * @return metacommand, might be empty
     */
    static std::string getNextMetacommand(std::istream& in);

protected:

    /**
     * @brief Write a metacommand
     * 
     * This function will write the command prepending a '!' and adjusting the case.
     * 
     * @param out stream
     * @param cmd metacommand (without !)
     */
    static void writeMetacommand(std::ostream& out, const std::string& cmd);

    /**
     * @brief Match an optional metacommand
     * 
     * Try to match a metacommand. The function will succeed if the metacommand
     * is found or if there are no metacommands at the beginning of the stream.
     * 
     * An exception will be thrown in case an unexpected metacommand is encountered.
     * 
     * @param in stream
     * @param cmd metacommand (without !)
     */
    static void matchMetacommandOptional(std::istream& in, const std::string& cmd);

    /**
     * @brief Match a metacommand
     * 
     * Try to match a metacommand. The function will succeed if the metacommand
     * is found.
     * 
     * An exception will be thrown in case an unexpected metacommand is encountered 
     * or if there are no metacommands at the beginning of the stream.
     * 
     * @param in stream
     * @param cmd metacommand (without !)
     */
    static void matchMetacommandRequired(std::istream& in, const std::string& cmd);

};

/**
 * @brief Stream insertion operator for Streamable
 * 
 * @param out output stream
 * @param streamable streamable object to insert
 * @return output stream
 */
std::ostream& operator<<(std::ostream& out, const Streamable& streamable);

/**
 * @brief Stream extraction operator for Layer
 * 
 * @param in input stream
 * @param streamable streamable object where to extract into
 * @return input stream
 */
std::istream& operator>>(std::istream& in, Streamable& streamable);

}

/**
 * @brief Create a Streamable object from an input stream
 * 
 * @tparam T a copiable Streamable type
 * @param in input stream
 * @return the new Streamable object
 */
template <typename T>
T createFromStream(std::istream& in) {
    T t{};
    t.fromStream(in);
    return t;
}

}

#endif