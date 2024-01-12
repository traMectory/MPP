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

using json = nlohmann::json;

typedef double NT;
// typedef long long NT;
typedef CGAL::Exact_predicates_exact_constructions_kernel K;
// typedef CGAL::Cartesian<NT> K;
typedef CGAL::Polygon_with_holes_2<K> Polygon_with_holes;
typedef CGAL::Polygon_2<K> Polygon;
typedef Polygon::Vertex_iterator VertexIterator;
typedef Polygon::Edge_const_iterator EdgeIterator;
typedef K::Point_2 Point;
typedef K::Segment_2 Segment;
typedef CGAL::Bbox_2 Bbox;

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

    long long score = 0;

    long time;

public:

    Problem();

    Problem(char *file_name);

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

    void setTime(long t) { time = t; };
    void addComment(std::string c) { comments.push_back(c); };

    // void addToPacking(Polygon c) { packing_polygons.push_back(c); };

    // void loadSolution();
    void loadSolution(char *file_name);

    std::string getString() { return name; };
    int getNumItems() { return num_items; };

    void setItems(std::vector<Item*> itemsN) { items = itemsN; };

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