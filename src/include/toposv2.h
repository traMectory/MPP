#pragma once
#include "solver.h"
#include "problem.hpp"
#include <CGAL/minkowski_sum_2.h>

enum joinMode { EXACT, LEFT_EXTEND };

class Toposv2 : public Solver
{
public:
    Toposv2() {};

    bool bestPlacement(Polygon& next, Polygon& leftContainer, Polygon& rightContainer, std::vector<Iso_rectangle>& bBoxes, Iso_rectangle& placedBBox, Point& translation, NT& bestEval);

    NT evalPlacement(Polygon& next, Point& position, Polygon& leftContainer, Polygon& rightContainer, std::vector<Iso_rectangle>& bBoxes, Iso_rectangle& placedBBox);

    void addNewPieceExact(Candidate& next, Polygon& leftContainer, Polygon& rightContainer);

    void addNewPieceRightBounds(Candidate& next, Polygon& leftContainer, Polygon& rightContainer);

    SolveStatus solve(Problem* prob);

    joinMode joinMode = LEFT_EXTEND;
};

