#pragma once
#include "solver.h"
#include "problem.hpp"
#include "clipper2/clipper.h"
#include <execution>

typedef Clipper2Lib::Paths64 Paths;
typedef Clipper2Lib::Path64 Path;
typedef Clipper2Lib::Point64 Point64;

static void intersectZCallback(const Point64& e1bot, const Point64& e1top,
    const Point64& e2bot, const Point64& e2top, Point64& pt)
{
    const Point64 *first, *second;

    //std::cout << e1top.z << " " << e1bot.z << "\n";

    if (e1top.z == e1bot.z + 1) {
        first = &e1bot;
        second = &e1top;
    }
    else if (e1bot.z == e1top.z + 1) {
        first = &e1top;
        second = &e1bot;
    }
    else if (e1top.z < e1bot.z) {
        first = &e1bot;
        second = &e1top;
    }
    else
    {
        first = &e1top;
        second = &e1bot;
    }
    if (first->y < second->y) {
        pt.x -= 0;
        if (first->x < second->x) {
            pt.y += 1;
        }
        else if (first->x > second->x) {
            pt.y -= 0;
        }
    }
    else if (first->y > second->y) {
        pt.x += 1;
        if (first->x < second->x) {
            pt.y += 1;
        }
        else if (first->x > second->x) {
            pt.y -= 0;
        }
    }
    else {
        pt.y = first->y;
    }
}


enum class PlacementStrategy {
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    TOP_LEFT,
    TOP_RIGHT,
    MIN_DIST,
    CONCAVE_FIT,
    TOPOS
};

enum class ComparisonResult {
    BETTER,
    WORSE,
    EQUAL
};

struct EdgeVector {
    Point64 vec;
    double slope;
    EdgeVector(Point64 v){
        vec = v;
        slope = v.x == 0 ? std::numeric_limits<double>::max() : ((double)v.y) / ((double)v.x);
    }
    EdgeVector() {};
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
    Point64 bottomLeft;
    Point64 topRight;
};

struct BoundingBox {
    Point64 bottomLeft;
    Point64 topRight;
    Point64 center;
};

class IncrementalNoFitSolver : public Solver
{
protected:
    Problem* problem;
    Polygon container;
    std::vector<Item*> items;
    Path pathContainer;
    std::vector<Item*> unusedItems;


    Paths placedPieces;
    std::list<BoundingBox> placedBBoxes;

    std::list<ItemWithNoFit*> itemsWithNoFit;

    BoundingBox BBoxPlaced;

    void initNoFits(size_t index);

    void additionalInits() {
        Point64 bottomLeftPlaced = Point64(std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::max());
        Point64 topRightPlaced = Point64(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::min());
        BBoxPlaced = { bottomLeftPlaced, topRightPlaced, {0,0} };
    };

    void updateNoFits(ItemWithNoFit* addedPiece, Point64& translation);

    void additionalUpdates(ItemWithNoFit* addedPiece, Point64& translation);

    bool findBestItem(ItemWithNoFit* &bestItem, Point64& translation);

    bool findBestPlacement(ItemWithNoFit* testedItem, Point64& attachmentPoint, int64_t& eval);

    bool ItemIsBetter(ItemWithNoFit* item1, Point64& translation1, int64_t eval1, ItemWithNoFit* item2, Point64& translation2, int64_t eval2);

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

    static void getBestFitOrder(Problem *problem);

    bool bestFit = false;

    size_t batchSize = 99;

    int64_t scaleFactor = 1;

    PlacementStrategy placementMode = PlacementStrategy::BOTTOM_LEFT;
   
    bool reorderItems = false;

    bool DEBUG = false;
    bool VERBOSE = false;
};