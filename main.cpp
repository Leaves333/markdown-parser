#include "parser.cpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

// converts the markdown document [input_file] into an html document.
// prints to stdout if no [output_file] is provided,
// otherwise writes to [output_file].
const char *usage_format = "%s [input_file] [-o output_file]\n";

int main(int argc, char *argv[]) {
    // hello world i'm writing some code!
    std::cout << "hello world!" << std::endl;

    int opt;
    std::string output_file = "";

    while ((opt = getopt(argc, argv, "o:")) != -1) {
        switch (opt) {
        case 'o':
            output_file = optarg;
            break;
        default:
            fprintf(stderr, usage_format, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, usage_format, argv[0]);
        exit(EXIT_FAILURE);
    }

    std::string input_file = argv[optind];
    std::vector<std::string> document = read_lines(input_file);
    Root ast = parse_ast(document);
    std::string html = render_html(ast);

    if (output_file == "") {
        std::cout << html;
    } else {
        std::ofstream fout(output_file);
        fout << html;
    }

    printf("input_file = %s\n", argv[optind]);
    printf("output_file = %s\n", output_file.c_str());
}
