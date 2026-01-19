// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include "parser.h"
#include "types.h"
#include <catch2/catch.hpp>
#include <vector>

using namespace std;

TEST_CASE("read_lines works correctly", "[read_lines]") {
    vector<string> expected;
    auto lines = {
        "# hello world!",
        "",
        ("this is a markdown file. it has some lines of text. hooray! this is "
         "one long"),
        "paragraph split over multiples lines.",
        "",
        "this is a short line. with *italics* and **bold**.",
    };
    for (string line : lines) {
        expected.push_back(line);
    }

    REQUIRE(expected == read_lines("./tests/hello_world.md"));
}

TEST_CASE("join_lines works correctly", "[join_lines]") {
    string expected = "# hello world! "
                      " "
                      "this is a markdown file. it has some lines of text. "
                      "hooray! this is one long "
                      "paragraph split over multiples lines. "
                      " "
                      "this is a short line. with *italics* and **bold**.";

    vector<string> lines = read_lines("./tests/hello_world.md");
    REQUIRE(expected == join_lines(lines, " "));
}

TEST_CASE("parse_text works on simple input", "[parse_text]") {
    REQUIRE(parse_text("hello world!").value == Text{"hello world!"}.value);
}

TEST_CASE("parse_heading works on simple input", "[parse_heading]") {
    Heading h = parse_heading("# This is a big heading");
    vector<Node> expected_children = {Text{"This is a big heading"}};
    REQUIRE(h.depth == 1);
    REQUIRE(h.children == expected_children);

    h = parse_heading("##### This is a small heading");
    expected_children = {Text{"This is a small heading"}};
    REQUIRE(h.depth == 5);
    REQUIRE(h.children == expected_children);
}

TEST_CASE("parse_heading errors on invalid depths", "[parse_heading]") {
    REQUIRE_THROWS(parse_heading("this is actually not a header."));
    REQUIRE_THROWS(parse_heading("####### this header has too much depth!"));
}

TEST_CASE("parse_heading recognizes phrasing content in title",
          "[parse_heading]") {

    Heading h = parse_heading("# title *with emphasis* **and strong**");
    vector<Node> expected_children = {
        Text{"title "},
        Emphasis{vector<Node>{Text{"with emphasis"}}},
        Text{" "},
        Strong{vector<Node>{Text{"and strong"}}},
    };

    REQUIRE(h.depth == 1);
    REQUIRE(h.children == expected_children);
}

TEST_CASE("parse_phrasing_content works on simple inputs",
          "[parse_phrasing_content]") {

    SECTION("parses a single block of text") {
        string content = "hello world!";
        vector<Node> expected_nodes = {Text{"hello world!"}};
        REQUIRE(expected_nodes == parse_phrasing_content(content));
    }

    SECTION("parses a single block of inline code") {
        string content = "`hello world!`";
        vector<Node> expected_nodes = {InlineCode{"hello world!"}};
        REQUIRE(expected_nodes == parse_phrasing_content(content));
    }

    SECTION("parses a single block of emphasis asterisks") {
        string content = "*hello world!*";
        vector<Node> expected_nodes = {
            Emphasis{vector<Node>({Text{"hello world!"}})}};
        REQUIRE(expected_nodes == parse_phrasing_content(content));
    }

    SECTION("parses a single block of emphasis underscores") {
        string content = "_hello world!_";
        vector<Node> expected_nodes = {
            Emphasis{vector<Node>({Text{"hello world!"}})}};
        REQUIRE(expected_nodes == parse_phrasing_content(content));
    }

    SECTION("parses a single block of strong asterisks") {
        string content = "**hello world!**";
        vector<Node> expected_nodes = {
            Strong{vector<Node>({Text{"hello world!"}})}};
        REQUIRE(expected_nodes == parse_phrasing_content(content));
    }

    SECTION("parses a single block of strong underscores") {
        string content = "__hello world!__";
        vector<Node> expected_nodes = {
            Strong{vector<Node>({Text{"hello world!"}})}};
        REQUIRE(expected_nodes == parse_phrasing_content(content));
    }
}

TEST_CASE("parse_phrasing_content parses multiple blocks in a row",
          "[parse_phrasing_content]") {
    string content =
        "text text text *emphasis* __strong__`inline` trailing text";
    vector<Node> expected_nodes = {Text{"text text text "},
                                   Emphasis{vector<Node>{Text{"emphasis"}}},
                                   Text{" "},
                                   Strong{vector<Node>{Text{"strong"}}},
                                   InlineCode{"inline"},
                                   Text{" trailing text"}};
    REQUIRE(expected_nodes == parse_phrasing_content(content));
}

TEST_CASE("parse_phrasing_content handles nested blocks",
          "[parse_phrasing_content]") {
    string content = "**strong *emphasis* strong**";
    vector<Node> expected_nodes = {Strong{
        vector<Node>{Text{"strong "}, Emphasis{vector<Node>{Text{"emphasis"}}},
                     Text{" strong"}}}};
    REQUIRE(expected_nodes == parse_phrasing_content(content));
}

TEST_CASE("parse_ast works on simple inputs", "[parse_ast]") {
    vector<string> document = {
        "# heading",
        "",
        "paragraph paragraph blah blah blah",
        "this should all be one single block",
    };
    Root expected = Root{vector<Node>{
        Heading{1, vector<Node>{Text{"heading"}}},
        Paragraph{vector<Node>{Text{"paragraph paragraph blah blah blah this "
                                    "should all be one single block"}}}}};
    REQUIRE(expected == parse_ast(document));

    document = read_lines("./tests/hello_world.md");
    expected = Root{vector<Node>{
        Heading{1, vector<Node>{Text{"hello world!"}}},
        Paragraph{vector<Node>{
            Text{"this is a markdown file. it has some lines of text. hooray! "
                 "this is one long paragraph split over multiples lines."}}},
        Paragraph{vector<Node>{
            Text{"this is a short line. with "},
            Emphasis{vector<Node>{Text{"italics"}}},
            Text{" and "},
            Strong{vector<Node>{Text{"bold"}}},
            Text{"."},
        }}}};
    REQUIRE(expected == parse_ast(document));
}
