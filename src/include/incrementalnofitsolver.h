#pragma once
#include "solver.h"
#include "problem.hpp"
#include <CGAL/minkowski_sum_2.h>
#include "clipper2/clipper.h"

struct ItemWithNoFit {
    Item* item;
    Paths innerFit;
    std::map<int, Paths> noFits;
};

class IncrementalNoFitSolver : public Solver
{
protected:
    Problem* problem;
    Path container;
    std::vector<Item*> items;

    bool containerEmpty = true;


    Paths placedPieces;

    std::list<ItemWithNoFit*> itemsWithNoFit;

    Point bottomLeft;

    void precomputeNoFits(Item* item, Path& inversePoly, std::map<int, Paths>& result);

    void initNoFits(size_t index);

    void additionalInits() {
        NT xmin = std::numeric_limits<NT>::max(), ymin = std::numeric_limits<NT>::max();
        for (auto p: problem->getContainer())
        {
            if (p.x < xmin)
                xmin = p.x;
            if (p.y < ymin)
                ymin = p.y;
        }
        bottomLeft = Point(xmin, ymin);
    };

    void updateNoFits(Item* addedPiece, Point& translation);

    void additionalUpdates(Item* addedPiece, Point& translation) {};

    bool findBestItem(ItemWithNoFit* &bestItem, Path& placedPoly, Point& translation);

    bool findBestPlacement(ItemWithNoFit* testedItem, Point& attachmentPoint);

    int64_t evalPlacement(Path& placedPoly, Point& translation, int value);

    bool evalIsBetter(int64_t first, int64_t second) {
        //returns true iff first position eval is better than second
        return first < second;
    };

    void addNewPiece(Item* item, Path& placedPoly, Point& translation);
public:
    IncrementalNoFitSolver() {};

    SolveStatus solve(Problem* prob);

    //static void getHullVacancies(const Polygon& poly, std::vector<Polygon>& vacancies);

    size_t batchSize = 2090;

    bool DEBUG = false;
};

