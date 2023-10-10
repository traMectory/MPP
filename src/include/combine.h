#pragma once

#include "solver.h"
#include "msum.h"
// #include "problem.hpp"
#include <iostream>
#include <iterator>

#include <CGAL/minkowski_sum_2.h>
#include <CGAL/Polygon_vertical_decomposition_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/ch_graham_andrew.h>

typedef std::list<Polygon_with_holes> Pwh_list;

class Combine : public Solver
{
public:
    Combine(){};

    SolveStatus solve(Problem *prob);
};