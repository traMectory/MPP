#ifndef CORNER_H
#define CORNER_H

#include "problem.hpp"
#include "solver.h"
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/tags.h>

enum Strategy {
    BL, TL, BR, TR
};

class Corner : public Solver {
private:
    std::vector<Strategy> strategies;
    Problem *prob;
    SolveStatus corner();
    Polygon_with_holes pack_into(Item *item, Polygon_with_holes c, Strategy strategy);
    Polygon translate_by(Polygon& p, Point& t);
    Polygon inverse(Polygon& p);
public:
    Corner(std::vector<Strategy> strategies);

    SolveStatus solve(Problem *prob);
};


#endif