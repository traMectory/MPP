#pragma once

#include "problem.hpp"
#include "miscellaneous.h"

#include <vector>

class Solver {

public:
    virtual ~Solver() {};

    virtual SolveStatus solve(Problem* poly) = 0;
};