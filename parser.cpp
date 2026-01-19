#include "parser.h"
#include "types.h"
#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

// parses content as generic markdown content, splitting it into a list of Nodes
std::vector<Node> parse_phrasing_content(const std::string &content) {

    const std::string EMPHASIS_ASTERISK = "*";
    const std::string STRONG_ASTERISK = "**";
    const std::string EMPHASIS_UNDERSCORE = "_";
    const std::string STRONG_UNDERSCORE = "__";
    const std::string INLINE_CODE = "`";

    // list of tokens we should check when parsing the markdown content.
    // order here determines precedence when generating the AST
    const std::vector<std::string> TOKENS = {
        INLINE_CODE, STRONG_ASTERISK, STRONG_UNDERSCORE, EMPHASIS_ASTERISK,
        EMPHASIS_UNDERSCORE};

    // while we haven't processed the whole std::string...
    std::vector<Node> nodes;
    size_t pos = 0;
    std::string text_buffer = "";
    while (pos < content.length()) {

        bool found_match = false;
        for (std::string token : TOKENS) {

            // is this the start of a valid token?
            if (content.substr(pos, token.length()) != token) {
                continue;
            }

            // is there a closing token of this type?
            size_t matching_pos = content.find(token, pos + token.length());
            if (matching_pos == std::string::npos) {
                continue;
            }

            // push text_buffer node if it is not empty
            if (text_buffer.length() > 0) {
                nodes.push_back(Text{text_buffer});
                text_buffer = "";
            }

            // everything between the delimiters is marked
            std::string marked = content.substr(
                pos + token.length(), matching_pos - pos - token.length());
            if (token == INLINE_CODE) {
                nodes.push_back(InlineCode{marked});
            } else if (token == EMPHASIS_ASTERISK ||
                       token == EMPHASIS_UNDERSCORE) {
                std::vector<Node> emphasis_nodes =
                    parse_phrasing_content(marked);
                nodes.push_back(Emphasis{emphasis_nodes});
            } else if (token == STRONG_ASTERISK || token == STRONG_UNDERSCORE) {
                std::vector<Node> strong_nodes = parse_phrasing_content(marked);
                nodes.push_back(Strong{strong_nodes});
            } else {
                throw std::runtime_error(
                    "parse phrasing content tried to check an invalid token!");
            }

            pos = matching_pos + token.length();
            found_match = true;
            break;
        }

        if (!found_match) {
            text_buffer += content[pos];
            pos++;
        }
    }

    // push text_buffer node if it is not empty
    if (text_buffer.length() > 0) {
        nodes.push_back(Text{text_buffer});
    }

    return nodes;
}

Text parse_text(const std::string &line) { return Text{line}; }

// parses `line` as if it was a heading, and returns the generated Node
Heading parse_heading(const std::string &line) {
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
std::vector<std::string> read_lines(std::string filename) {
    std::ifstream file;
    file.open(filename);

    std::vector<std::string> lines;
    std::string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

// joins each line of `lines` together with a space.
// should work similarly to `" ".join(lines)` in python
std::string concatenate_lines(const std::vector<std::string> &lines) {
    std::string joined = "";
    for (int i = 0; i < lines.size(); i++) {
        joined += lines[i];
        if (i != lines.size() - 1) {
            joined += " ";
        }
    }
    return joined;
}
