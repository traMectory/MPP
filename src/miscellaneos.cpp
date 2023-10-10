#include "miscellaneous.h"

void parseOptions(int argc, char* argv[]) {
    cxxopts::Options options("mpp <file_name>", "Attempts to solve the MPP");

    options.add_options()
        ("h, help", "show this help message")
        ("v, visualize", "automatically open ipe at the end of the calculation")
        ("a, algorithm", "chooses the algorithm to use (0: msum, 1: msumone, 2: leftlayer, 3: leftlayerone)", cxxopts::value<int>());
        // ("s, solution", "load an existing solution from ../Solutions with the same name or a specified name", cxxopts::value<std::string>()->default_value(""));
        // ("t", "tmp_dictionary", cxxopts::value<std::string>()->default_value("D:/GIT/C++/geowordle_core_cmake"))

    auto result = options.parse(argc, argv);

    // if (argc != 2 ) {
    //     std::cout << "Please specify exactly one file name.\n";
    //     exit(0);
    // }

    if (result.count("help"))
    {
        std::cout << options.help();
        exit(0);
    }

    if (result.count("visualize"))
        visualize = true;

    if (result.count("algorithm")) {
        algorithm = result["algorithm"].as<int>();
    }
}
