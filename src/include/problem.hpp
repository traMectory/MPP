#pragma once

#include <fstream>
#include <vector>
#include <fstream>
#include <json.hpp>
#include <iostream>
#include "graphIPE.hpp"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Bbox_2.h>

using json = nlohmann::json;

typedef CGAL::Exact_predicates_exact_constructions_kernel K;
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
    Polygon poly;
    int quantity;
    int value;
};

class Problem
{
private:
    std::string name;
    std::string type;
    std::string comment;

    Polygon container;

    std::vector<Polygon> candidates;

    std::vector<Item> items;

    int num_items;

    long long score;

    long time;

public:
    // Problem(Polygon poly)
    // {
    //     name = "temp";
    //     type = "temp";
    //     boundary_polgyon = poly;
    // };

    Problem(char *file_name);

    Problem(Polygon cont, std::vector<Polygon> cands)
    {
        container = cont;
        num_items = 0;
        for (Polygon c : cands)
        {
            items.push_back(Item(c, 1, 1));
            num_items++;
        }
    };

    void setTime(long t) { time = t; };
    void setComment(std::string c) { comment = c; };

    // void addToPacking(Polygon c) { packing_polygons.push_back(c); };

    // void loadSolution();
    void loadSolution(char *file_name);

    std::string getString() { return name; };
    int getNumItems() { return num_items; };

    bool isValidPacking();

    bool test() { return true; };

    void addCandidate(Polygon cand, int value) { candidates.push_back(cand); score += value; };

    Polygon getContainer() { return container; };
    std::vector<Item> getItems() { return items; };
    std::vector<Polygon> getCandidates() { return candidates; };

    // void sortItemsDensity() { std::sort(items.begin(), items.end(), compareItems); };

    void loadSolution(std::string solution_name);
    void visualize();
    void visualizeSolution();
    void output();
};