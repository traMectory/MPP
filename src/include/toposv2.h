#pragma once
#include "solver.h"
#include "problem.hpp"
#include <CGAL/minkowski_sum_2.h>

enum class joinType { EXACT, APPROXIMATE, LEFT_EXTEND };

void toIPE2(std::string path, Polygon boundary, std::vector<Polygon> polygons, std::vector<Point> points);

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

    joinType joinMode = joinType::EXACT;
    bool DEBUG = false;
    bool VERBOSE = false;
};

