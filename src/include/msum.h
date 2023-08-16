#pragma once

#include "solver.h"
#include "problem.hpp"

#include <CGAL/minkowski_sum_2.h>
#include <CGAL/Polygon_vertical_decomposition_2.h>
#include <CGAL/Boolean_set_operations_2.h>
typedef std::list<Polygon_with_holes> Pwh_list;

class Msum : public Solver
{
public:
    Msum(){};

    SolveStatus solve(Problem *prob);
};