#include "problem.hpp"
#include "solver.h"
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/tags.h>

class BottomLeft : public Solver {
public:
    BottomLeft(){};

    SolveStatus solve(Problem *prob);

private:
    SolveStatus bl(Problem *prob);

    Polygon_with_holes pack_into(Item *item, Polygon_with_holes c, Problem *prob, int i);

    Polygon translate_by(Polygon& p, Point& t);

    Polygon inverse(Polygon& p);
};