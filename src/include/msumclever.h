#pragma once

#include "solver.h"
#include "problem.hpp"

#include <CGAL/minkowski_sum_2.h>
#include <CGAL/Polygon_triangulation_decomposition_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/connect_holes.h>

typedef std::list<Polygon_with_holes> Pwh_list;

class MsumClever : public Solver
{
public:
    MsumClever(){};

    SolveStatus solve(Problem *prob);
};