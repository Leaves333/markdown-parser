#include <fstream>
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

struct InlineCode {
    string value;
    bool operator==(const InlineCode &) const = default;
};

struct Emphasis {
    vector<Node> children;
    bool operator==(const Emphasis &) const = default;
};

struct Strong {
    vector<Node> children;
    bool operator==(const Strong &) const = default;
};

struct Heading {
    int depth;
    vector<Node> children;
    bool operator==(const Heading &) const = default;
};

struct Node
    : variant<Root, Paragraph, Heading, Text, InlineCode, Emphasis, Strong> {
    using variant::variant;
};

// parses content as generic markdown content, splitting it into a list of Nodes
vector<Node> parse_phrasing_content(const string &content) {

    // BUG: this should find the earliest matching pair in the string,
    // the way priority is currently implemented doesn't work

    const string EMPHASIS_ASTERISK = "*";
    const string STRONG_ASTERISK = "**";
    const string EMPHASIS_UNDERSCORE = "_";
    const string STRONG_UNDERSCORE = "__";
    const string INLINE_CODE = "`";

    // list of tokens we should check when parsing the markdown content.
    // order here determines precedence when generating the AST
    const vector<string> TOKENS = {INLINE_CODE, STRONG_ASTERISK,
                                   STRONG_UNDERSCORE, EMPHASIS_ASTERISK,
                                   EMPHASIS_UNDERSCORE};

    // while we haven't processed the whole string...
    vector<Node> nodes;
    size_t pos = 0;
    while (pos < content.length()) {

        // loop through all possible tokens and try to find matching pairs
        bool found_match = false;
        for (string token : TOKENS) {

            size_t first = content.find(token, pos);
            if (first == string::npos)
                continue;
            size_t second = content.find(token, first + token.length());
            if (second == string::npos)
                continue;

            // both tokens are right text to each other, no text being marked
            if (second - first == token.length())
                continue;

            // we found two valid positions
            // everything up until the first delimiter is text
            string text_prefix = content.substr(pos, first - pos);
            if (text_prefix.length() > 0) {
                nodes.push_back(Text{text_prefix});
            }

            // everything between the delimiters is marked
            string marked = content.substr(first + token.length(),
                                           second - first - token.length());
            if (token == INLINE_CODE) {
                nodes.push_back(InlineCode{marked});
            } else if (token == EMPHASIS_ASTERISK ||
                       token == EMPHASIS_UNDERSCORE) {
                vector<Node> emphasis_nodes = {Node{Text{marked}}};
                nodes.push_back(Emphasis{emphasis_nodes});
            } else if (token == STRONG_ASTERISK || token == STRONG_UNDERSCORE) {
                vector<Node> strong_nodes = {Node{Text{marked}}};
                nodes.push_back(Strong{strong_nodes});
            } else {
                throw runtime_error(
                    "parse phrasing content tried to check an invalid token!");
            }

            pos = second + token.length();
            found_match = true;
            break;
        }

        // if no tokens were found, the rest of the string is all text
        if (!found_match) {
            Node remaining_text = Node{Text{content.substr(pos)}};
            nodes.push_back(remaining_text);
            break;
        }
    }

    return nodes;
}

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

// joins each line of `lines` together with a space.
// should work similarly to `" ".join(lines)` in python
string concatenate_lines(const vector<string> &lines) {
    string joined = "";
    for (int i = 0; i < lines.size(); i++) {
        joined += lines[i];
        if (i != lines.size() - 1) {
            joined += " ";
        }
    }
    return joined;
}
