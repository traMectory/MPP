#pragma once
#include "solver.h"
#include "problem.hpp"
#include <CGAL/minkowski_sum_2.h>
#include "clipper2/clipper.h"

typedef Clipper2Lib::Paths64 Paths;
typedef Clipper2Lib::Path64 Path;
typedef Clipper2Lib::Point64 Point64;

struct ItemWithNoFit {
    Item* item;
    Paths innerFit;
    std::vector<Polygon> inversePoly;
    std::vector<Polygon> convexDecomp;
};

class IncrementalNoFitSolver : public Solver
{
protected:
    Problem* problem;
    Polygon container;
    std::vector<Item*> items;

    bool containerEmpty = true;


    Paths placedPieces;

    std::list<ItemWithNoFit*> itemsWithNoFit;

    Point bottomLeft;

    void precomputeNoFits(Item* item, Path& inversePoly, std::map<int, Paths>& result);

    void initNoFits(size_t index);

    void additionalInits() {
        NT xmin = std::numeric_limits<long>::max(), ymin = std::numeric_limits<long>::max();
        for (auto p: problem->getContainer())
        {
            if (p.x() < xmin)
                xmin = p.x();
            if (p.y() < ymin)
                ymin = p.y();
        }
        bottomLeft = Point(xmin, ymin);
    };

    void updateNoFits(ItemWithNoFit* addedPiece, Point64& translation);

    void additionalUpdates(ItemWithNoFit* addedPiece, Point64& translation) {};

    bool findBestItem(ItemWithNoFit* &bestItem, Point64& translation);

    bool findBestPlacement(ItemWithNoFit* testedItem, Point64& attachmentPoint);

    int64_t evalPlacement(Polygon& placedPoly, Point64& translation, int value);

    bool evalIsBetter(int64_t first, int64_t second) {
        //returns true iff first position eval is better than second
        return first < second;
    };

    void addNewPiece(ItemWithNoFit* item, Point64& translation);
public:
    IncrementalNoFitSolver() {};

    SolveStatus solve(Problem* prob);

    //static void getHullVacancies(const Polygon& poly, std::vector<Polygon>& vacancies);

    size_t batchSize = 2090;

    bool DEBUG = false;
};

