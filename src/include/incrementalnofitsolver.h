#pragma once
#include "solver.h"
#include "problem.hpp"
#include <CGAL/minkowski_sum_2.h>

struct ItemWithNoFit {
    Item* item;
    Polygon_with_holes noFit;
    Polygon inversePoly;
};

class IncrementalNoFitSolver : public Solver
{
protected:
    bool containerEmpty = true;

    Problem* problem;

    Polygon placedPieces;

    std::list<ItemWithNoFit*> itemsWithNoFit;

    Point bottomLeft;

    void initNoFits(size_t index);

    virtual void additionalInits() { bottomLeft = Point(problem->getContainer().left_vertex()->x(), problem->getContainer().bottom_vertex()->y()); };

    void updateNoFits(Candidate& addedCand);

    virtual void additionalUpdates(Candidate& addedCand) {};

    bool findBestItem(ItemWithNoFit* &bestItem, Polygon& placedPoly, Point& translation);

    virtual bool findBestPlacement(ItemWithNoFit* testedItem, Point& attachmentPoint);

    virtual NT evalPlacement(Polygon& placedPoly, int value);

    virtual bool evalIsBetter(NT first, NT second) {
        //returns true iff first position eval is better than second
        return first < second;
    };

    void addNewPiece(Item* item, Polygon& placedPoly, Point& translation);
public:
    IncrementalNoFitSolver() {};

    SolveStatus solve(Problem* prob);

    static void getHullVacancies(const Polygon& poly, std::vector<Polygon>& vacancies);

    size_t batchSize = 2090;

    bool DEBUG = false;
};

