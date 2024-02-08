#pragma once

#include <fstream>
#include <vector>
#include <fstream>
#include <json.hpp>
#include <iostream>
#include "graphIPE.hpp"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Bbox_2.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Quotient.h>
#include <CGAL/CORE_BigRat.h>
#include <CGAL/minkowski_sum_2.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/property_map.h>
#include <CGAL/Small_side_angle_bisector_decomposition_2.h>
#include <CGAL/partition_2.h>
#include <CGAL/Partition_traits_2.h>

using json = nlohmann::json;

//typedef double NT;
typedef CGAL::Exact_predicates_exact_constructions_kernel K;
//typedef CGAL::Cartesian<NT> K;
typedef K::RT NT;
typedef CGAL::Polygon_with_holes_2<K> Polygon_with_holes;
typedef CGAL::Polygon_2<K> Polygon;
typedef Polygon::Vertex_iterator VertexIterator;
typedef Polygon::Edge_const_iterator EdgeIterator;
typedef CGAL::Point_2<K> Point;
typedef K::Line_2 Line;
typedef K::Segment_2 Segment;
typedef CGAL::Bbox_2 Bbox;
typedef CGAL::Iso_rectangle_2<K> Iso_rectangle;
typedef Polygon::Vertex_circulator VertexCirculator;
typedef CGAL::Aff_transformation_2<K> Transformation;
typedef CGAL::Vector_2<K> Vector;
// typedef CGAL::Convex_hull_traits_adapter_2<K, CGAL::Pointer_property_map<Point>::type > Convex_hull_traits;
typedef CGAL::Partition_traits_2<K, CGAL::Pointer_property_map<K::Point_2>::type > Partition_traits;


struct Edge
{
    Point s;
    Point t;
};

struct Item
{
    int index;
    Polygon poly;
    int quantity;
    int value;

};

struct Candidate
{
    int index;
    Polygon poly;
    NT x_translation;
    NT y_translation;
};

struct GASpecifics {
    size_t populationSize;
    size_t generations;
    std::vector<long long> fitness;
    double mutationRate;
    double crossoverRate;
    std::string mutationType;
    std::string crossoverType;

    json toJSON() {
        json hyperparams;
        hyperparams["population-size"] = populationSize;
        hyperparams["generations"] = generations;
        hyperparams["fitness"] = {};
        for (auto f : fitness) {
            hyperparams["fitness"].push_back(f);
        }
        hyperparams["mutation-rate"] = mutationRate;
        hyperparams["crossover-rate"] = crossoverRate;
        hyperparams["mutation-type"] = mutationType;
        hyperparams["crossover-type"] = crossoverType;
        return hyperparams;
    };

    void fromJSON(json params) {
        if (params.contains("population-size"))
            populationSize = params["population-size"];
        else
            std::cerr << "no population-size defined in config";
        if (params.contains("generations"))
            generations = params["generations"];
        else
            std::cerr << "no generations defined in config";
        if (params.contains("mutation-rate"))
            mutationRate = params["mutation-rate"];
        else
            std::cerr << "no mutation-rate defined in config";
        if (params.contains("crossover-rate"))
            crossoverRate = params["crossover-rate"];
        else
            std::cerr << "no crossover-rate defined in config";
        if (params.contains("mutation-type"))
            mutationType = params["mutation-type"];
        else
            std::cerr << "no mutation-type defined in config";
        if (params.contains("crossover-type"))
            crossoverType = params["crossover-type"];
        else
            std::cerr << "no crossover-type defined in config";
    }
};

class Problem
{
private:
    std::string name;
    std::string type;
    std::vector<std::string> comments;

    Polygon container;

    std::vector<Candidate> candidates;

    std::vector<Item*> items;

    int num_items;

    int num_candidates = 0;

    long long score = 0;

    long time;
    std::string algorithmType;
    std::string placementStrategy;
    json algorithmSpecifics;

public:

    Problem();

    Problem(char* file_name);

    Problem(Polygon cont, std::vector<Polygon> cands)
    {
        container = cont;
        num_items = 0;
        for (Polygon c : cands)
        {
            Item* it = new Item();
            it->poly = c;
            it->quantity = 1;
            it->value = 1;
            items.push_back(it);
            num_items++;
        }
    };
    Problem(Polygon cont, std::vector<Item*> cands, int nnum_items)
    {
        container = cont;
        num_items = nnum_items;
        items = cands;
    };

    long long getScore() { return score; };
    void setScore(long long nscore) { score = nscore; };

    void setRunningTime(long t) { time = t; };
    void setAlgorithmType(std::string t) { algorithmType = t; };
    void setPlacementStrategy(std::string s) { placementStrategy = s; };
    std::string getPlacementStrategy() { return placementStrategy; };
    void setAlgorithmSpecifics(json s) { algorithmSpecifics = s; };
    void setAlgorithmSpecifics(GASpecifics h) { algorithmSpecifics = h.toJSON(); };
    void addComment(std::string c) { comments.push_back(c); };
    std::vector<std::string> getComments() { return comments; };

    // void addToPacking(Polygon c) { packing_polygons.push_back(c); };

    // void loadSolution();
    void loadSolution(char* file_name);

    std::string getString() { return name; };
    int getNumItems() { return num_items; };
    int getNumCandidates() { return num_candidates; };

    void setItems(std::vector<Item*> itemsN) { items = itemsN; num_items = items.size(); };

    bool isValidPacking();

    bool test() { return true; };

    void addCandidate(Candidate cand, int value) { candidates.push_back(cand); score += value; };
    void clearCandidates() { candidates.clear(); };

    Polygon getContainer() { return container; };
    std::vector<Item*> getItems() { return items; };
    std::vector<Candidate> getCandidates() { return candidates; };
    void prettyPrint() { std::cout << " - Layed " << candidates.size() << " items with a total value of " << score << std::endl; };

    // void sortItemsDensity() { std::sort(items.begin(), items.end(), compareItems); };

    void loadSolution(std::string solution_name);
    void visualize();
    void visualizeSolution();

    void roundItems();

    void storeSolution(std::string loc);
    void storeSolution(std::string loc, int g);

    void output();
};
