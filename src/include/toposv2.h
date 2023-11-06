#pragma once
#include "solver.h"
#include "problem.hpp"
#include <CGAL/minkowski_sum_2.h>

enum placementPolicy { WASTE, OVERLAP, DISTANCE };

class Toposv2 : public Solver
{
public:
    Toposv2() {};

    bool bestPlacement(Polygon& next, Polygon& leftContainer, Polygon& rightContainer, Point& translation);

    NT evalPlacement(Polygon& next, Point& position, Polygon& leftContainer, Polygon& rightContainer);

    SolveStatus solve(Problem* prob);

    placementPolicy placementPolicy = OVERLAP;
};

