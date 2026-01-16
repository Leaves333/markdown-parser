#include <fstream>
#include <libintl.h>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

using namespace std;

// forward declare Node type
struct Node;

struct Root {
    vector<Node> children;
    bool operator==(const Root &) const = default;
};

struct Paragraph {
    vector<Node> children;
    bool operator==(const Paragraph &) const = default;
};

struct Text {
    string value;
    bool operator==(const Text &) const = default;
};

struct Heading {
    int depth;
    vector<Node> children;
    bool operator==(const Heading &) const = default;
};

struct Node : variant<Root, Paragraph, Heading, Text> {
    using variant::variant;
};

Text parse_text(const string &line) { return Text{line}; }

// parses `line` as if it was a heading, and returns the generated Node
Heading parse_heading(const string &line) {
    int depth = 0;
    while (depth < line.length() && line[depth] == '#') {
        depth++;
    }

    if (depth < 1 || depth > 6) {
        throw std::invalid_argument(
            "line passed into parse_heading() had an invalid depth");
    }

    Heading heading;
    heading.depth = depth;

    Text contents = parse_text(line.substr(depth));
    heading.children.push_back(Node{contents});

    return heading;
}

// opens `filename`, reads it line by line,
// and returns a vector of the lines read
vector<string> read_lines(string filename) {
    ifstream file;
    file.open(filename);

    vector<string> lines;
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

unsigned int factorial(unsigned int x) {
    return x <= 1 ? 1 : factorial(x - 1) * x;
}
