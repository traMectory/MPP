#pragma once

#include "cxxopts.hpp"
#include <fstream>
#include "problem.hpp"

extern bool visualize;
extern std::string solution;
extern std::string comment;
extern int algorithm;
extern GASpecifics GAHyperparams;

enum class SolveStatus { Optimal, Feasible, Unsolved };

void parseOptions(int argc, char* argv[]);