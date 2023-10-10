#pragma once

#include "cxxopts.hpp"

extern bool visualize;
extern std::string solution;
extern std::string comment;
extern int algorithm;

enum class SolveStatus { Optimal, Feasible, Unsolved };

void parseOptions(int argc, char* argv[]);