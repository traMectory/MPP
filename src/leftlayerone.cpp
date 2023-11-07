#include "leftlayerone.h"

bool compareItemsArea(Item *i1, Item *i2)
{
    return i1->poly.area() < i2->poly.area();
};

SolveStatus LeftLayerOne::solve(Problem *prob)
{
    prob->addComment("Algorithm: leftlayer");
    prob->addComment("Sorting: area inc");

    long int inter = 0;

    Polygon container = prob->getContainer();

    NT wall = 10e5;

    std::vector<Item *> copyItems = prob->getItems();

    std::sort(copyItems.begin(), copyItems.end(), compareItemsArea);

    // std::cout << "2\n";
    int count = 0;
    int num_items = 0;
    for (Item *item : copyItems)
    {
        num_items += item->quantity;
    }
    // std::cout << "2\n";

    std::cout << "total number of items: " << num_items << std::endl;

    bool added = true;

    while (added)
    {

        // Polygon_with_holes target(outer_target);
        Polygon target;
        target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymax() + wall));
        target.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymax() + wall));
        target.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymin() - wall));
        target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymin() - wall));

        // std::cout << target.is_simple() << container.is_simple() << std::endl;

        // std::cout << container.is_counterclockwise_oriented() << std::endl;

        auto v = container.right_vertex()[0];
        int n = 0;
        int i = container.size() - 1;
        bool adding = false;
        // std::cout << container.size() << std::endl;
        while (i > n)
        {
            if (!adding && container[i] == v)
            {
                adding = true;
                n -= (container.size() - i);
                // std::cout << i << std::endl;
            }

            if (adding)
            {
                // std::cout << i % container.size() << std::endl;
                target.push_back(container[i < 0 ? i + container.size() : i]);
            }

            i--;
        }
        // std::cout << n << std::endl;
        target.push_back(Point(container[i < 0 ? i + container.size() : i].x(), container[i < 0 ? i + container.size() : i].y()));
        target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymin() - wall + 1));

        // Problem test(target, {});
        // test.visualizeSolution();
        // break;

        // std::cout << target.is_simple() << std::endl;

        // Problem temp(target, {});
        // temp.visualizeSolution();

        // for (int i = 0< container.size(); i++) {
        //     target.push_back(container.right_vertex()[i]);
        // }

        // target.add_hole(container);

        added = false;

        auto bestDistance = 10e8 * CGAL::squared_distance(Point(0, 0), Point(0, 1));
        Point bestTrans;
        Polygon bestCand;
        Polygon bestMod;
        Item *bestItem;
        bool bestUp;

        for (Item *item : copyItems)
        {
            if (item->quantity == 0)
                continue;

            // std::cout << "(" << count << "/" << num_items << ")"
            //           << " q: " << item->quantity << ", v: " << item->value << ", v/a: " << (item->value / item->poly.area());
            count += item->quantity;

            Polygon inverse;
            Point left = item->poly.left_vertex()[0];

            for (int i = 0; i < item->poly.size(); i++)
            {
                Point p = item->poly.vertices()[i];

                auto newP = p - left;
                inverse.push_back(Point(-newP.x(), -newP.y()));
            }

            Polygon_with_holes sum = CGAL::minkowski_sum_2(target, inverse);

            // Problem tmp(outer_target, {});
            // tmp.addCandidate(container, 1);
            // tmp.visualizeSolution();

            Pwh_list freeSpace;
            for (Polygon hole : sum.holes())
            {
                freeSpace.push_back(Polygon_with_holes(hole));
            }

            // for (Polygon candidate : prob->getCandidates())
            // {
            //     std::cout << candidate.is_simple() << std::endl;
            //     Polygon_with_holes sum2 = CGAL::minkowski_sum_2(candidate, inverse);

            //     Pwh_list intR;
            //     for (Polygon_with_holes free : freeSpace)
            //     {
            //         CGAL::difference(free, sum2, std::back_inserter(intR));
            //     }

            //     freeSpace = intR;
            // }

            if (freeSpace.empty())
            {
                item->quantity = 0;
                continue;
            }

            Polygon cand;
            Point t = item->poly.vertices()[0];
            Point l = freeSpace.front().outer_boundary().left_vertex()[0];

            auto trans = l - left;

            for (Point v : item->poly)
            {
                cand.push_back(v + trans);
            }

            Polygon mod;

            Point top = Point(10e10, -10e10);
            int top_i;
            Point bottom = Point(10e10, 10e10);
            int bottom_i;

            for (int i = 0; i < cand.size(); i++)
            {
                Point v = cand[i];

                if (v.y() > top.y() || (v.y() == top.y() && v.x() < top.x()))
                {
                    top = v;
                    top_i = i;
                }

                if (v.y() < bottom.y() || (v.y() == bottom.y() && v.x() > bottom.x()))
                {
                    bottom = v;
                    bottom_i = i;
                }
            }

            top_i = top_i < bottom_i ? top_i + cand.size() : top_i;

            mod.push_back(Point(top.x() - 10e16, top.y()));
            mod.push_back(Point(bottom.x() - 10e16, bottom.y()));
            for (int i = 0; (i + bottom_i) <= top_i; i++)
            {
                mod.push_back(cand[(i + bottom_i) % cand.size()]);
            }

            // std::list<Polygon_with_holes> intersection;
            // std::list<Polygon_with_holes> difference;

            // CGAL::intersection(mod, container, std::back_inserter(intersection));

            // if (intersection.size() != 1 || intersection.front().has_holes())
            // {
            //     Problem tmp(containecandr, {});
            //     tmp.addCandidate(mod, 1);
            //     tmp.visualizeSolution();
            //     exit(0);
            //     std::cout << "!!!!! not good 152 !!!!!!";
            // }

            // CGAL::difference(intersection.front(), cand, std::back_inserter(difference));

            // if (difference.size() != 1 || difference.front().has_holes())
            // {
            //     std::cout << "!!!!! not good 159 !!!!!!";
            //     Problem temp(intersection.front().outer_boundary(), {});
            //     temp.addCandidate(intersection.front().holes()[0], 1);

            //     temp.visualizeSolution();
            // }

            // auto distance = difference.empty() ? 0 : difference.front().outer_boundary().area();
            NT distance = 0;

            // std::cout << "1 " << cand.vertices().size() << "\n";

            // // Point candLeftUp = cand.left_vertex()[cand.vertices().size()-1];

            // Point candLeft = cand.left_vertex()[0];
            // // Apperantly leftvertex +1 cannot be done at the last vertex
            // Point candLeftNext = candLeft != *(cand.vertices_end() - 1) ? cand.left_vertex()[1] : cand.left_vertex()[-1];
            // // Point candLeftMid = CGAL::midpoint(candLeft, candLeftMid);

            // std::cout << candLeft << " - " << std::endl;

            // std::cout << "2\n";
            // auto distance = 10e20 * CGAL::squared_distance(Point(0, 0), Point(0, 1));

            // std::cout << "2.5\n";
            // bool up = false;
            // ;

            // for (Segment edge : container.edges())
            // {
            //     // auto dis = CGAL::squared_distance(edge, candLeft) + CGAL::squared_distance(edge, candLeftUp);

            //     // if (dis < distance) {
            //     //     distance = dis;
            //     //     up = true;
            //     // }

            //     std::cout << "2.6\n";
            //     std::cout << candLeft << " " << candLeftNext << std::endl;
            //     std::cout << edge << " " << std::endl;
            //     auto dis = CGAL::squared_distance(edge, candLeft) + CGAL::squared_distance(edge, candLeftNext);

            //     std::cout << "2.7\n";
            //     if (dis < distance)
            //     {
            //         distance = dis;
            //         up = false;
            //     }
            // }

            if (distance < bestDistance)
            {
                bestDistance = distance;
                bestCand = cand;
                bestItem = item;
                bestMod = mod;
                bestTrans = Point(trans.x(), trans.y());
                added = true;
                break;
            }
        }

        if (added)
        {

            std::list<Polygon_with_holes> newContainer;
            CGAL::difference(container, bestMod, std::back_inserter(newContainer));

            container = newContainer.front().outer_boundary();

            // Point candLeft = bestCand.left_vertex()[0];
            // Apperantly leftvertex +1 cannot be done at the last vertex

            // bool swapped = candLeft != *(bestCand.vertices_end() - 1);
            // Point candLeftNext = swapped ? bestCand.left_vertex()[1] : bestCand.left_vertex()[-1];

            // Point candLeftNext;

            // if (candLeft != *(bestCand.vertices_end() - 1))
            // {
            //     candLeftNext = bestCand.left_vertex()[1];
            // }
            // else
            // {
            //     candLeftNext = candLeft;
            //     candLeft = bestCand.left_vertex()[-1];
            // }

            // prob->addCandidate(mod, 1);
            Candidate bestC;
            bestC.index = bestItem->index;
            bestC.poly = bestCand;
            bestC.x_translation = (int) (bestTrans.x().interval().pair().first + bestTrans.x().interval().pair().second) / 2;
            bestC.y_translation = (int) (bestTrans.y().interval().pair().first + bestTrans.y().interval().pair().second) / 2;
            prob->addCandidate(bestC, bestItem->value);
            bestItem->quantity--;
        }
    }

    // std::cout << 

    return SolveStatus::Feasible;
}