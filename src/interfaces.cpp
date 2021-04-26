#include "cpphots/interfaces.h"

#include <stdexcept>


namespace cpphots {

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

void Streamable::matchMetacommandOptional(std::istream& in, const std::string& cmd) {

    skip_whitespace(in);

    char ex = in.peek();

    if (ex != '!') {
        return;
    }

    in.get();
    std::string line;
    std::getline(in, line);
    if (line != to_upper(cmd)) {
        throw std::runtime_error("Wrong metacommand: expected '" + to_upper(cmd) + "', got '" + line + "'");
    }

}

void Streamable::matchMetacommandRequired(std::istream& in, const std::string& cmd) {

    skip_whitespace(in);

    char ex = in.peek();

    if (ex != '!') {
        throw std::runtime_error("Wrong metacommand: expected '" + to_upper(cmd) + "', nothing found");
    }

    in.get();
    std::string line;
    std::getline(in, line);
    if (line != to_upper(cmd)) {
        throw std::runtime_error("Wrong metacommand: expected '" + to_upper(cmd) + "', got '" + line + "'");
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