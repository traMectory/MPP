#pragma once
#include "solver.h"
#include "problem.hpp"
#include <CGAL/minkowski_sum_2.h>

void toIPE2(std::string path, Polygon boundary, std::vector<Polygon> polygons, std::vector<Point> points);

enum joinMode { EXACT, APPROXIMATE, LEFT_EXTEND };

class Toposv2 : public Solver
{
public:
    Toposv2() {};

    bool bestPlacement(Polygon& next, Polygon& container, std::vector<Iso_rectangle>& bBoxes, Iso_rectangle& placedBBox, Point& translation, NT& bestEval);

    NT evalPlacement(Polygon& next, Point& position, Polygon& container, std::vector<Iso_rectangle>& bBoxes, Iso_rectangle& placedBBox);

    void addNewPieceExact(Candidate& next, Polygon& container);

    void addNewPieceApprox(Candidate& next, Polygon& container);

    void addNewPieceRightBounds(Candidate& next, Polygon& leftContainer, Polygon& rightContainer);

    SolveStatus solve(Problem* prob);

    joinMode joinMode = EXACT;
    bool DEBUG = false;
    bool VERBOSE = false;
};

