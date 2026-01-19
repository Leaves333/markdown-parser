#pragma once

#include <ostream>
#include <string>
#include <variant>
#include <vector>

// forward declare node struct
struct Node;

struct Root {
    std::vector<Node> children;
    bool operator==(const Root &) const = default;
};

struct Paragraph {
    std::vector<Node> children;
    bool operator==(const Paragraph &) const = default;
};

struct Text {
    std::string value;
    bool operator==(const Text &) const = default;
};

struct InlineCode {
    std::string value;
    bool operator==(const InlineCode &) const = default;
};

struct Emphasis {
    std::vector<Node> children;
    bool operator==(const Emphasis &) const = default;
};

struct Strong {
    std::vector<Node> children;
    bool operator==(const Strong &) const = default;
};

struct Heading {
    int depth;
    std::vector<Node> children;
    bool operator==(const Heading &) const = default;
};

struct Node : std::variant<Root, Paragraph, Heading, Text, InlineCode, Emphasis,
                           Strong> {
    using variant::variant;
};

std::ostream &operator<<(std::ostream &os, const Node &node);

namespace debug {

// NOTE: hacky solution to print out vectors
template <class T>
inline std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
    os << "vector={";
    for (int i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if (i < vec.size() - 1) {
            os << ", ";
        }
    }
    os << "}";
    return os;
}

} // namespace debug
