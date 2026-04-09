#include "parser.h"
#include "types.h"
#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

const std::string EMPTY_STRING = "";

// converts `block` into a Paragraph via parse_phrasing_content,
// pushes the converted Paragraph into `root`,
// and sets `block` to "". does nothing if `block` is empty.
void push_block(Root &root, std::string &block) {
    if (block == EMPTY_STRING) {
        return;
    }
    Paragraph p;
    p.children = parse_phrasing_content(block);
    root.children.push_back(p);
    block = EMPTY_STRING;
}

Root parse_ast(const std::vector<std::string> &lines) {

    Root root;
    std::string block = EMPTY_STRING;
    for (const std::string &line : lines) {
        if (line == EMPTY_STRING) {
            push_block(root, block);
        } else if (line[0] == '#') {
            push_block(root, block);
            root.children.push_back(parse_heading(line));
        } else {
            if (block == EMPTY_STRING) {
                block = line;
            } else {
                block += " " + line;
            }
        }
    }

    push_block(root, block);
    return root;
}

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
    heading.children = parse_phrasing_content(line.substr(depth + 1));

    return heading;
}

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

std::string join_lines(const std::vector<std::string> &lines, const std::string delimiter) {
    std::string joined = "";
    for (int i = 0; i < lines.size(); i++) {
        joined += lines[i];
        if (i != lines.size() - 1) {
            joined += delimiter;
        }
    }
    return joined;
}

namespace render {

struct Renderer {

    std::string operator()(const Root &n) {
        std::vector<std::string> header_lines = read_lines("./data/header.html");
        std::vector<std::string> footer_lines = read_lines("./data/footer.html");
        std::string header = join_lines(header_lines, "\n");
        std::string footer = join_lines(footer_lines, "\n");

        std::string content = EMPTY_STRING;
        for (const Node &child : n.children) {
            content += render_html(child);
        }

        return header + content + footer;
    }

    std::string operator()(const Paragraph &n) {
        std::string html = "<p>";
        for (const Node &child : n.children) {
            html += render_html(child);
        }
        html += "</p>";
        return html;
    }

    std::string operator()(const Heading &n) {
        std::string html = "<h" + std::to_string(n.depth) + ">";
        for (const Node &child : n.children) {
            html += render_html(child);
        }
        html += "</h" + std::to_string(n.depth) + ">";
        return html;
    }

    std::string operator()(const Emphasis &n) {
        std::string html = "<em>";
        for (const Node &child : n.children) {
            html += render_html(child);
        }
        html += "</em>";
        return html;
    }

    std::string operator()(const Strong &n) {
        std::string html = "<strong>";
        for (const Node &child : n.children) {
            html += render_html(child);
        }
        html += "</strong>";
        return html;
    }

    std::string operator()(const Text &n) { return n.value; }

    std::string operator()(const InlineCode &n) {
        return "<code>" + n.value + "</code>";
    }

};

} // namespace render

std::string render_html(const Node &n) {
    return std::visit(render::Renderer{}, n);
}
