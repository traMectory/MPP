#include "combine.h"

bool compareItems2(Item *i1, Item *i2)
{
    return i1->poly.bbox().y_span() * i1->poly.bbox().x_span() > i2->poly.bbox().y_span() * i2->poly.bbox().x_span();
};

struct ItemMulti
{
    Polygon bound;
    std::vector<Polygon> polys;
    long long value;
};

SolveStatus Combine::solve(Problem *prob)
{

    // CGAL::Polygon_vertical_decomposition_2<K> ssab_decomp;

    // std::vector<Item *> copyItems = prob->getItems();

    // std::vector<Item *> newItems;

    // std::sort(copyItems.begin(), copyItems.end(), compareItems2);
    // // CGAL::Polygon_vertical_decomposition_2<K> ssab_decomp;

    // for (int i = 0; i < copyItems.size(); i++)
    // {
    //     Item *item = copyItems[i];
    //     if (item->quantity <= 0)
    //         continue;

    //     if (item->poly.is_convex())
    //     {
    //         newItems.push_back(new Item(item->poly, {}, item->quantity, item->value));
    //         item->quantity = 0;
    //         continue;
    //     }

    //     Polygon convex;

    //     std::list<Point> points;
    //     CGAL::ch_graham_andrew(item->poly.vertices_begin(), item->poly.vertices_end(), std::back_inserter(points));
    //     for (Point pt : points)
    //     {
    //         convex.push_back(pt);
    //     }

    //     for (int k = 0; k < item->quantity; k++)
    //     {
    //         Problem test(convex, {});

    //         test.addCandidate(item->poly, 1);
    //         // test.setItems({copyItems.begin() + k, std::min(copyItems.begin() + k + 1000, copyI));

    //         Msum psolver = Msum();
    //         auto b = psolver.solve(&test);

    //         // for (Item* it : copyItems) {
    //         //     std::cout << it->quantity << std::endl;
    //         // }
    //         // test.visualizeSolution();

    //         long wall = 10e3;

    //         // Polygon outer_target;
    //         // outer_target.push_back(Point(convex.bbox().xmin() - wall, convex.bbox().ymin() - wall));
    //         // outer_target.push_back(Point(convex.bbox().xmax() + wall, convex.bbox().ymin() - wall));
    //         // outer_target.push_back(Point(convex.bbox().xmax() + wall, convex.bbox().ymax() + wall));
    //         // outer_target.push_back(Point(convex.bbox().xmin() - wall, convex.bbox().ymax() + wall));

    //         // Polygon_with_holes target(outer_target);
    //         // target.add_hole(convex);

    //         Item *multi = new Item(convex, test.getCandidates(), 1, item->value);

    //         for (Polygon candis : test.getCandidates())
    //         {
    //             multi->inners.push_back(candis);
    //         }

    //         newItems.push_back(multi);
    //         item->quantity--;
    //     }
    // }

    // // Problem test2(prob->getContainer(), {});
    // prob->setItems(newItems);

    // Msum psolver = Msum();
    // auto b = psolver.solve(prob);

    return SolveStatus::Feasible;
}
