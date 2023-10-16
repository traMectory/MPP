#pragma once
#include "solver.h"
#include "problem.hpp"
#include <CGAL/minkowski_sum_2.h>
#include <CGAL/draw_polygon_2.h>

class Topos : public Solver
{
public:
    Topos() {};

    bool bestPlacement(Polygon& placed, Polygon& next, Polygon& leftContainer, Polygon& rightContainer, Point &translation);

    float evalPlacement(Polygon& placed, Polygon& next, Point& position, Polygon& leftContainer, Polygon& rightContainer);

    SolveStatus solve(Problem* prob);
};

