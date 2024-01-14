#pragma once
#include "solver.h"
#include "problem.hpp"
#include "clipper2/clipper.h"

typedef Clipper2Lib::Paths64 Paths;
typedef Clipper2Lib::Path64 Path;
typedef Clipper2Lib::Point64 Point64;

static void intersectZCallback(const Point64& e1bot, const Point64& e1top,
    const Point64& e2bot, const Point64& e2top, Point64& pt)
{
    const Point64 *first, *second;
    pt.z = 1;
    if (e1bot.z == 1 || e1top.z == 1) {
        if (&e1top == (&e1bot + 1)) {
            first = &e1bot;
            second = &e1top;
        }
        else if (&e1bot == (&e1top + 1)) {
            first = &e1top;
            second = &e1bot;
        }
        else if (&e1top < &e1bot) {
            first = &e1bot;
            second = &e1top;
        }
        else
        {
            first = &e1top;
            second = &e1bot;
        }
    }
    else if (e2bot.z == 1 || e2top.z == 1) {
        if (&e1top == (&e1bot + 1)) {
            first = &e1bot;
            second = &e1top;
        }
        else if (&e1bot == (&e1top + 1)) {
            first = &e1top;
            second = &e1bot;
        }
        else if (&e1top < &e1bot) {
            first = &e1bot;
            second = &e1top;
        }
        else
        {
            first = &e1top;
            second = &e1bot;
        }
    }
    else {
        return;
    }
    if (first->y < second->y) {
        pt.x -= 1;
    }
    else if (first->y > second->y) {
        pt.x += 1;
    }
    else {
        pt.y = first->y;
    }
}

enum PlacementStrategy {
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    TOP_LEFT,
    TOP_RIGHT,
    MIN_DIST
};

enum ComparisonResult {
    BETTER,
    WORSE,
    EQUAL
};

struct EdgeVector {
    Point64 vec;
    double slope;
};

struct ConvexEdgeList {
    Point64 start;
    std::vector<EdgeVector> rightEdges;
    std::vector<EdgeVector> leftEdges;
};

struct ItemWithNoFit {
    Item* item;
    Paths innerFit;
    Path rightEdge;
    std::vector<ConvexEdgeList> convexDecomp;
    std::vector<ConvexEdgeList> inversePoly;
};

class IncrementalNoFitSolver : public Solver
{
protected:
    Problem* problem;
    Polygon container;
    std::vector<Item*> items;
    Path pathContainer;


    Paths placedPieces;

    std::list<ItemWithNoFit*> itemsWithNoFit;

    Point bottomLeft;

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

    void additionalUpdates(ItemWithNoFit* addedPiece, Point64& translation);

    bool findBestItem(ItemWithNoFit* &bestItem, Point64& translation);

    bool findBestPlacement(ItemWithNoFit* testedItem, Point64& attachmentPoint);

    int64_t evalPlacement(ItemWithNoFit* item, Point64& translation);

    bool tieBreak(ItemWithNoFit* first, ItemWithNoFit* second) {
        if (first->item->value > second->item->value)
            return true;
        else
            return false;
    };

    ComparisonResult compareEval(int64_t first, int64_t second) {
        //returns true iff first position eval is better than second
        if (first < second)
            return ComparisonResult::BETTER;
        if (first > second)
            return ComparisonResult::WORSE;
        else
            return ComparisonResult::EQUAL;

    };

    void addNewPiece(ItemWithNoFit* item, Point64& translation);


public:
    IncrementalNoFitSolver() {};

    SolveStatus solve(Problem* prob);

    //static void getHullVacancies(const Polygon& poly, std::vector<Polygon>& vacancies);

    bool bestFit = true;

    size_t batchSize = 999999;

    int64_t scaleFactor = 1000000;

    PlacementStrategy placementMode = MIN_DIST;

    bool DEBUG = false;
    bool VERBOSE = true;
};

