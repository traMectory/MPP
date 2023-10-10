#include "leftlayer.h"

#include <chrono>

#define tms std::chrono::high_resolution_clock::now()
#define dif(a, b) std::chrono::duration_cast<std::chrono::microseconds>(a - b)
#define difp(a, b) std::chrono::duration_cast<std::chrono::microseconds>(a + b)

SolveStatus LeftLayer::solve(Problem *prob)
{
    auto total = tms;

    auto diff = dif(total, total);
    long int inter = 0;
    auto tsum = dif(total, total);

    Polygon container = prob->getContainer();

    long wall = -10e6;

    Polygon outer_target;
    outer_target.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymin() - wall));
    outer_target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymin() - wall));
    outer_target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymax() + wall));
    outer_target.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymax() + wall));

    std::vector<Item *> copyItems = prob->getItems();

    int count = 0;
    int num_items = 0;
    for (Item *item : copyItems)
    {
        num_items += item->quantity;
    }

    std::cout << "total number of items: " << num_items << std::endl;

    bool added = true;

    while (added)
    {

        Polygon_with_holes target(outer_target);
        target.add_hole(container);

        added = false;

        auto bestDistance = 10e20 * CGAL::squared_distance(Point(0, 0), Point(0, 1));
        Polygon bestCand;
        Polygon bestMod;
        Item *bestItem;
        bool bestUp;

        for (Item *item : copyItems)
        {
            if (item->quantity == 0)
                continue;

            count += item->quantity;
            
            Polygon inverse;
            Point left = item->poly.left_vertex()[0];

            for (int i = 0; i < item->poly.size(); i++)
            {
                Point p = item->poly.vertices()[i];

                auto newP = p - left;
                inverse.push_back(Point(-newP.x(), -newP.y()));
            }

            auto ssum = tms;
            CGAL::Polygon_triangulation_decomposition_2<K> ssab_decomp;
            Polygon_with_holes sum = CGAL::minkowski_sum_2(target, inverse, ssab_decomp);
            tsum += dif(tms, ssum);


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

            auto tdiff = tms;

            Polygon mod;

            Point top = Point(10e25, -10e25);
            int top_i;
            Point bottom = Point(10e25, 10e25);
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


            mod.push_back(Point(top.x() - 10e15, top.y()));
            mod.push_back(Point(bottom.x() - 10e15, bottom.y()));
            for (int i = 0; (i + bottom_i) <= top_i; i++)
            {
                mod.push_back(cand[(i + bottom_i) % cand.size()]);
            }

            std::list<Polygon_with_holes> intersection;
            std::list<Polygon_with_holes> difference;


            CGAL::intersection(mod, container, std::back_inserter(intersection));

            if (intersection.size() != 1 || intersection.front().has_holes())
            {
                std::cout << "!!!!! not good 152 !!!!!!";
            }

            CGAL::difference(intersection.front(), cand, std::back_inserter(difference));
            

            // if (difference.size() != 1 || difference.front().has_holes())
            // {
            //     std::cout << "!!!!! not good 159 !!!!!!";
            //     Problem temp(intersection.front().outer_boundary(), {});
            //     temp.addCandidate(intersection.front().holes()[0], 1);

            //     temp.visualizeSolution();
            // }

            auto distance = difference.empty() ? 0 : difference.front().outer_boundary().area();

            diff += dif(tms, tdiff);

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
            bestC.poly = bestCand;
            prob->addCandidate(bestC, bestItem->value);
            bestItem->quantity--;
        }
    }

    return SolveStatus::Feasible;
}