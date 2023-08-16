#include "msumclever.h"

SolveStatus MsumClever::solve(Problem *prob)
{
    // Point p(0, 0.3), q, r(2, 0.9);

    // {
    //     q = Point(1, 0.6);
    //     // std::cout << (CGAL::collinear(p, q, r) ? "collinear\n" : "not collinear\n");
    // }
    // {
    //     std::istringstream input("0 0.3   1 0.6   2 0.9");
    //     input >> p >> q >> r;
    //     // std::cout << (CGAL::collinear(p, q, r) ? "collinear\n" : "not collinear\n");
    // }
    // {
    //     q = CGAL::midpoint(p, r);
    //     // std::cout << (CGAL::collinear(p, q, r) ? "collinear\n" : "not collinear\n");

    //     double a = q.x().approx().pair().first();

    //     // std::cout << a << std::endl;
    //     // std::cout << q.y() << std::endl;
    // }

    Polygon container = prob->getContainer();

    long wall = 10e6;

    Polygon outer_target;
    outer_target.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymin() - wall));
    outer_target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymin() - wall));
    outer_target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymax() + wall));
    outer_target.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymax() + wall));

    std::vector<Item> copyItems = prob->getItems();

    CGAL::Polygon_triangulation_decomposition_2<K> ssab_decomp;

    int count = 0;
    int num_items = 0;
    for (Item item : copyItems)
    {
        num_items += item.quantity;
    }

    while (true)
    {

        Polygon_with_holes target(outer_target);
        target.add_hole(container);

        Item *bestItem = nullptr;
        Polygon bestCand;
        auto bestDist = Point(0, DBL_MAX).y();

        // std::cout << 1 << std::endl;

        for (int i = 0; i < copyItems.size(); i++)
        {
            Item item = copyItems[i];
            if (item.quantity == 0)
            {
                continue;
            }
            // std::cout << 2 << std::endl;

            Polygon inverse;
            Point top = item.poly.top_vertex()[0];

            for (int i = 0; i < item.poly.size(); i++)
            {
                Point p = item.poly.vertices()[i];
                auto newP = p - top;
                inverse.push_back(Point(-newP.x(), -newP.y()));
            }

            // std::cout << 4 << std::endl;
            Polygon_with_holes sum = CGAL::minkowski_sum_2(target, inverse, ssab_decomp);

            if (sum.has_holes())
            {
                Pwh_list freeSpace;
                for (Polygon hole : sum.holes())
                {
                    freeSpace.push_back(Polygon_with_holes(hole));
                }

                // std::cout << 4 << std::endl;
                for (Polygon candidate : prob->getCandidates())
                {
                Polygon_with_holes sum2 = CGAL::minkowski_sum_2(candidate, inverse);

                Pwh_list intR;
                for (Polygon_with_holes free : freeSpace)
                {
                    CGAL::difference(free, sum2, std::back_inserter(intR));
                }

                freeSpace = intR;
                }

                if (freeSpace.empty())
                {
                    continue;
                }

                // std::cout << 5 << std::endl;
                Polygon cand;
                Polygon candE;
                Point t = item.poly.vertices()[0];
                Point l = freeSpace.front().outer_boundary().top_vertex()[0];

                Point highest;
                Point before;
                Point after;
                double maxY = DBL_MIN;
                auto trans = l - top;

                Point prev = item.poly.vertices()[item.poly.vertices().size()-1];
                Point prev2 = item.poly.vertices()[item.poly.vertices().size()-2];

                // std::cout << 6 << std::endl;
                for (Point v : item.poly)
                {
                    Point vp(v + trans);
                    cand.push_back(vp);

                    if (prev.y() > maxY)
                    {
                        maxY = vp.y().approx().pair().first;

                        before = prev2;
                        highest = prev;
                        after = vp;
                    }
                    prev2 = prev;
                    prev = vp;
                }
                // std::cout << 7 << std::endl;

                auto shortestDistancePoly = Point(0, DBL_MAX).y();
                for (auto edge : container.edges())
                {
                    auto dis = (CGAL::squared_distance(edge, highest) + CGAL::squared_distance(edge, before) +1) / item.poly.area();
                    if (dis < shortestDistancePoly)
                    {
                        shortestDistancePoly = dis;
                    }
                    dis = (CGAL::squared_distance(edge, highest) + CGAL::squared_distance(edge, after) + 1) / item.poly.area();
                    if (dis < shortestDistancePoly)
                    {
                        shortestDistancePoly = dis;
                    }
                }

                if (shortestDistancePoly < bestDist)
                {
                    bestItem = &copyItems[i];
                    bestCand = cand;
                    bestDist = shortestDistancePoly;
                }

                // prob->addCandidate(cand, item.value);

                // // std::cout << " added";
            }
        }

        // std::cout << 8 << std::endl;
        if (bestItem == nullptr)
            break;

        // std::cout << bestItem << std::endl;
        // std::cout << bestItem->value << std::endl;
        
        prob->addCandidate(bestCand, bestItem->value);
        // std::cout << 8.5 << std::endl;
        bestItem->quantity = bestItem->quantity - 1;
        // std::cout << 9 << std::endl;

        Polygon_with_holes temp(container);
        temp.add_hole(bestCand);
        // std::cout << 10 << std::endl;

        // prob->visualizeSolution();

        // Polygon newContainer;
        // std::list<Point> pts;
        // CGAL::connect_holes(temp, std::back_inserter(pts));
        // // std::cout << 11 << std::endl;

        // for (Point pt : pts) {
        //     newContainer.push_back(pt);
        // }

        // Problem test(newContainer, {});
        // test.visualizeSolution();

        // container = newContainer;
        // break;
    }

    // for (Item item : copyItems)
    // {
    //     // std::cout << "(" << count << "/" << num_items << ")"
    //               << " q: " << item.quantity << ", v: " << item.value << ", v/a: " << (item.value / item.poly.area());
    //     count += item.quantity;
    //     for (int i = 0; i < item.quantity; i++)
    //     {

    //         Polygon inverse;
    //         Point left = item.poly.left_vertex()[0];

    //         for (int i = 0; i < item.poly.size(); i++)
    //         {
    //             Point p = item.poly.vertices()[i];
    //             auto newP = p - left;
    //             inverse.push_back(Point(-newP.x(), -newP.y()));
    //         }

    //         Polygon_with_holes sum = CGAL::minkowski_sum_2(target, inverse, ssab_decomp);

    //         if (sum.has_holes())
    //         {
    //             Pwh_list freeSpace;
    //             for (Polygon hole : sum.holes())
    //             {
    //                 freeSpace.push_back(Polygon_with_holes(hole));
    //             }

    //             for (Polygon candidate : prob->getCandidates())
    //             {
    //                 Polygon_with_holes sum2 = CGAL::minkowski_sum_2(candidate, inverse);

    //                 Pwh_list intR;
    //                 for (Polygon_with_holes free : freeSpace)
    //                 {
    //                     CGAL::difference(free, sum2, std::back_inserter(intR));
    //                 }

    //                 freeSpace = intR;
    //             }

    //             if (freeSpace.empty())
    //             {
    //                 continue;
    //             }

    //             Polygon cand;
    //             Point t = item.poly.vertices()[0];
    //             Point l = freeSpace.front().outer_boundary().left_vertex()[0];
    //             auto trans = l - left;

    //             for (Point v : item.poly)
    //             {
    //                 cand.push_back(v + trans);
    //             }

    //             prob->addCandidate(cand, item.value);

    //             // std::cout << " added";
    //         }
    //     }
    //     // std::cout << std::endl;
    //     // break;
    // }
    // std::cout << "doei\n";

    prob->visualizeSolution();
    return SolveStatus::Feasible;
}
