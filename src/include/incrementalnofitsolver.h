#pragma once
#include "solver.h"
#include "problem.hpp"
#include <CGAL/minkowski_sum_2.h>
#include "clipper2/clipper.h"

struct ItemWithNoFit {
    Item* item;
    Clipper2Lib::Path64 poly;
    Clipper2Lib::Paths64 innerFit;
    Clipper2Lib::Path64 inversePoly;
};

class IncrementalNoFitSolver : public Solver
{
protected:
    bool containerEmpty = true;

    Problem* problem;

    Clipper2Lib::Path64 placedPieces;

    std::list<ItemWithNoFit*> itemsWithNoFit;

    Point bottomLeft;

    void initNoFits(size_t index);

    void additionalInits() { bottomLeft = Point(problem->getContainer().left_vertex()->x(), problem->getContainer().bottom_vertex()->y()); };

    void updateNoFits(Clipper2Lib::Path64& addedPiece);

    void additionalUpdates(Clipper2Lib::Path64& addedPiece) {};

    bool findBestItem(ItemWithNoFit* &bestItem, Clipper2Lib::Path64& placedPoly, Clipper2Lib::Point64& translation);

    bool findBestPlacement(ItemWithNoFit* testedItem, Clipper2Lib::Point64& attachmentPoint);

    int64_t evalPlacement(Clipper2Lib::Path64& placedPoly, Clipper2Lib::Point64& translation, int value);

    bool evalIsBetter(int64_t first, int64_t second) {
        //returns true iff first position eval is better than second
        return first < second;
    };

    void addNewPiece(Item* item, Clipper2Lib::Path64& placedPoly, Clipper2Lib::Point64& translation);
public:
    IncrementalNoFitSolver() {};

    SolveStatus solve(Problem* prob);

    static void getHullVacancies(const Polygon& poly, std::vector<Polygon>& vacancies);

    size_t batchSize = 2090;

    bool DEBUG = false;
};

