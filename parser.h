#include "types.h"
#include <string>
#include <vector>

// given a sequence of lines of markdown, generates the AST for it
Root parse_ast(const std::vector<std::string> &lines);

// parses content as generic markdown content, splitting it into a list of Nodes
std::vector<Node> parse_phrasing_content(const std::string &content);

// converts a string into a Text node
Text parse_text(const std::string &line);

// parses `line` as if it was a heading, and returns the generated Node
Heading parse_heading(const std::string &line);

// opens `filename`, reads it line by line,
// and returns a vector of the lines read
std::vector<std::string> read_lines(std::string filename);

// joins each line of `lines` together with delimiter.
// should work like `delimiter.join(lines)` in python
std::string join_lines(const std::vector<std::string> &lines,
                       const std::string delimiter);

// renders a node to html
std::string render_html(const Node &n);
