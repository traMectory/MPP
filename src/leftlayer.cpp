#include "leftlayer.h"

SolveStatus LeftLayer::solve(Problem *prob)
{

    Point a(0, 103931928);
    Point b(3037902, 99751114);

    // Polygon a;
    // a.push_back(Point(0, 5));
    // a.push_back(Point(5, 0));
    // a.push_back(Point(10, 5));
    // a.push_back(Point(5, 10));

    // Polygon b;
    // b.push_back(Point(2, 3));
    // b.push_back(Point(3, 2));
    // b.push_back(Point(5, 7));
    // Pwh_list intR;
    // CGAL::difference(a, b, std::back_inserter(intR));

    // std::cout << intR.size() << std::endl;

    // return SolveStatus::Unsolved;
    Polygon bound(prob->getContainer());

    int count = 0;
    int num_items = 0;
    for (Item item : prob->getItems())
    {
        num_items += item.quantity;
    }

    for (Item item : prob->getItems())
    {
        std::cout << "(" << count << "/" << num_items << ")"
                  << " q: " << item.quantity << ", v: " << item.value << ", v/a: " << (item.value / item.poly.area());

        Point maxC;
        Point maxCp;
        auto maxY = bound.top_vertex()[0].y();
        for (int i = 0; i < bound.size(); i++)
        {
            if (bound[i].y() == (maxY))
            {
                maxC = bound[i];
                maxCp = bound[i != 0 ? i - 1 : bound.size() - 1];
            }
        }
        for (int i = 0; i < item.quantity; i++)
        {

            Polygon cand;
            Polygon candE;
            Point t = item.poly.top_vertex()[0];
            Point l = bound.top_vertex()[0];
            Point small = Point(0, 1);
            auto trans = l - t;

            for (Point v : item.poly)
            {
                auto a = v + trans;
                cand.push_back(a);
                if (a.y() != t.y() + trans.y())
                    candE.push_back(Point(a.x(), a.y() + 100));
                else
                    candE.push_back(a);
            }

            Pwh_list insideCheck;
            CGAL::difference(cand, bound, std::back_inserter(insideCheck));
            if (insideCheck.size() != 0)
            {
                std::cout << std::endl;
                continue;
            }

            Point maxP;
            Point maxPp;

            auto maxY = cand.top_vertex()[0].y();
            for (int i = 0; i < cand.size(); i++)
            {
                if (cand[i].y() == (maxY))
                {
                    maxP = cand[i];
                    maxPp = cand[i != 0 ? i - 1 : cand.size() - 1];
                }
            }

            // std::cout << "   " << maxC << " - " << maxCp << " - " << maxPp;

            // std::cout << " |" << CGAL::collinear(maxC, maxCp, maxPp) << "|" << std::endl;

            // Pwh_list insideCheck;
            // CGAL::difference(cand, bound, std::back_inserter(insideCheck));
            // if (insideCheck.size() != 0)
            // {
            //     std::cout << std::endl;
            //     continue;
            // }
            Pwh_list goodFit;
            CGAL::difference(bound, candE, std::back_inserter(goodFit));
            std::cout << " in";
            std::cout << " " << (goodFit.size() == 1) << " " << (!goodFit.front().has_holes()) << " " << goodFit.front().outer_boundary().is_simple();

            // if (goodFit.size() == 1 && !goodFit.front().has_holes() && goodFit.front().outer_boundary().is_simple())
            // {
            if (count == 47 || count == 56)
            {
                std::cout << goodFit.front().outer_boundary() << std::endl;
                Problem test(goodFit.front().outer_boundary(), {});
                test.addCandidate(candE, 1);
                test.visualizeSolution();
            }
            // }

            // Problem test(intR.front().outer_boundary(), {b});
            // test.visualizeSolution();

            // std::cout << intR.front().outer_boundary().is_simple() << std::endl;

            std::cout << std::endl;
        }
        count += item.quantity;
    }
    return SolveStatus::Unsolved;
}