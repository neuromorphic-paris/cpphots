#include "cpphots/interfaces/streamable.h"

#include <stdexcept>


namespace cpphots {

namespace interfaces {

std::string to_upper(std::string str) {

    for (size_t i = 0; i < str.size(); i++) {
        str[i] = std::toupper(str[i]);
    }
    return str;
}

void Streamable::writeMetacommand(std::ostream& out, const std::string& cmd) {

    out << "!" << to_upper(cmd) << std::endl;

}

void skip_whitespace(std::istream& in) {

    char ch;
    ch = in.peek();

    while (std::isspace(ch)) {
        in.get();
        ch = in.peek();
    }

}

std::string Streamable::getNextMetacommand(std::istream& in) {

    skip_whitespace(in);

    char ch = in.peek();

    if (ch != '!') {
        return "";
    }

    in.get();  // consume '!'

    std::string cmd;
    std::getline(in, cmd);
    return cmd;

}

void Streamable::matchMetacommandOptional(std::istream& in, const std::string& cmd) {

    auto meta = getNextMetacommand(in);

    if (meta == "") {
        return;
    }

    if (meta != to_upper(cmd)) {
        throw std::runtime_error("Wrong metacommand: expected '" + to_upper(cmd) + "', got '" + meta + "'");
    }

}

void Streamable::matchMetacommandRequired(std::istream& in, const std::string& cmd) {

    auto meta = getNextMetacommand(in);

    if (meta == "") {
        throw std::runtime_error("Wrong metacommand: expected '" + to_upper(cmd) + "', nothing found");
    }

    if (meta != to_upper(cmd)) {
        throw std::runtime_error("Wrong metacommand: expected '" + to_upper(cmd) + "', got '" + meta + "'");
    }

}

std::ostream& operator<<(std::ostream& out, const Streamable& streamable) {
    streamable.toStream(out);
    return out;
}

std::istream& operator>>(std::istream& in, Streamable& streamable) {
    streamable.fromStream(in);
    return in;
}

}

}