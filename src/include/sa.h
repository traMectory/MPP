#pragma once

#include "solver.h"
#include "problem.hpp"

class Sa : public Solver
{
public:
    Sa(){};

    SolveStatus solve(Problem *prob);
};