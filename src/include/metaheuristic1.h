#pragma once

#include "solver.h"
#include "problem.hpp"
#include "leftlayerone.h"
#include <algorithm>
#include <random>

class Metaheuristic : public Solver
{
public:
    Metaheuristic(){};

    SolveStatus solve(Problem *prob);
};