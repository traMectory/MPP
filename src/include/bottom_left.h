#ifndef BOTTOM_LEFT_H
#define BOTTOM_LEFT_H

#include "problem.hpp"
#include "solver.h"
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/tags.h>

class BottomLeft : public Solver {
public:
    BottomLeft(){};

    SolveStatus solve(Problem *prob);
};

SolveStatus bl(Polygon container, std::vector<Item*> items, std::vector<Candidate>* candidates, long long* score);
Polygon_with_holes pack_into(Item *item, Polygon_with_holes c, std::vector<Candidate>* candidates, long long* score, int i);
Polygon translate_by(Polygon& p, Point& t);
Polygon inverse(Polygon& p);

#endif