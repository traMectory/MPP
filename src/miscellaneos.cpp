#include "miscellaneous.h"

void parseOptions(int argc, char* argv[]) {
    cxxopts::Options options("mpp <file_name>", "Attempts to solve the MPP");

    options.add_options()
        ("h, help", "show this help message")
        ("v, visualize", "automatically open ipe at the end of the calculation")
        ("a, algorithm", "chooses the algorithm to use (0: msum, 1: leftlayerone, 2: Random orders)", cxxopts::value<int>());

    auto result = options.parse(argc, argv);

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
