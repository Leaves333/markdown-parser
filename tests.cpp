// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <vector>
#include "parser.cpp"

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

TEST_CASE("parse_text works on simple input", "[parse_text]") {
    REQUIRE(parse_text("hello world!").value == Text{"hello world!"}.value);
}

TEST_CASE("parse_heading works on simple input", "[parse_heading]") {
    Heading h = parse_heading("# This is a big heading");
    vector<Node> expected_children = {Text{" This is a big heading"}};
    REQUIRE(h.depth == 1);
    REQUIRE(h.children == expected_children);

    h = parse_heading("##### This is a small heading");
    expected_children = {Text{" This is a small heading"}};
    REQUIRE(h.depth == 5);
    REQUIRE(h.children == expected_children);
}

TEST_CASE("parse_heading errors on invalid depths", "[parse_heading]") {
    REQUIRE_THROWS(parse_heading("this is actually not a header."));
    REQUIRE_THROWS(parse_heading("####### this header has too much depth!"));
}

TEST_CASE("Factorials are computed", "[factorial]") {
    REQUIRE(factorial(0) == 1);
    REQUIRE(factorial(1) == 1);
    REQUIRE(factorial(2) == 2);
    REQUIRE(factorial(3) == 6);
    REQUIRE(factorial(10) == 3628800);
}
