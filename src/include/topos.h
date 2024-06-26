#pragma once
#include "solver.h"
#include "problem.hpp"
#include <CGAL/minkowski_sum_2.h>
#include <CGAL/draw_polygon_2.h>

enum class placementPolicy { WASTE, OVERLAP, DISTANCE };

class Topos : public Solver
{
public:
    Topos() {};

    bool bestPlacement(std::vector<Polygon>& placed, Polygon& next, Polygon& leftContainer, Polygon& rightContainer, Point& translation);

    NT evalPlacement(std::vector<Polygon>& placed, Polygon& next, Point& position, Polygon& leftContainer, Polygon& rightContainer);

    SolveStatus solve(Problem* prob);

    placementPolicy placementStrategy = placementPolicy::OVERLAP;
};

