#pragma once

#include "solver.h"
#include "problem.hpp"

#include <CGAL/Polygon_triangulation_decomposition_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/minkowski_sum_2.h>
typedef std::list<Polygon_with_holes> Pwh_list;

class LeftLayer : public Solver
{
public:
    LeftLayer(){};

    SolveStatus solve(Problem *prob);
};