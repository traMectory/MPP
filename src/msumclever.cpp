#include "msumclever.h"

bool compareItemsC(Item *i1, Item *i2)
{
    return i1->poly.area() > i2->poly.area();
};

SolveStatus MsumClever::solve(Problem *prob)
{
    std::cout << "clever\n";
    // Point p(0, 0.3), q, r(2, 0.9);

    // {
    //     q = Point(1, 0.6);
    //     std::cout << (CGAL::collinear(p, q, r) ? "collinear\n" : "not collinear\n");
    // }
    // {
    //     std::istringstream input("0 0.3   1 0.6   2 0.9");
    //     input >> p >> q >> r;
    //     std::cout << (CGAL::collinear(p, q, r) ? "collinear\n" : "not collinear\n");
    // }
    // {
    //     q = CGAL::midpoint(p, r);
    //     std::cout << (CGAL::collinear(p, q, r) ? "collinear\n" : "not collinear\n");

    //     double a = q.x().approx().pair().first();

    //     std::cout << a << std::endl;
    //     std::cout << q.y() << std::endl;
    // }

    Polygon container = prob->getContainer();

    long wall = 10e6;

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

    // Polygon outer_target;
    // outer_target.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymin() - wall));
    // outer_target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymin() - wall));
    // outer_target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymax() + wall));
    // outer_target.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymax() + wall));

    // Polygon_with_holes target(outer_target);
    // target.add_hole(container);

    std::vector<Item *> copyItems = prob->getItems();

    std::sort(copyItems.begin(), copyItems.end(), compareItemsC);

    // std::cout << "2\n";
    int count = 0;
    int num_items = 0;
    for (Item *item : copyItems)
    {
        num_items += item->quantity;
    }

    std::cout << "total number of items: " << num_items << std::endl;

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

        Pwh_list freeSpace;
        for (Polygon hole : sum.holes())
        {
            freeSpace.push_back(Polygon_with_holes(hole));
        }

        for (Candidate candidate : prob->getCandidates())
        {
            Polygon_with_holes sum2 = CGAL::minkowski_sum_2(candidate.poly, inverse);

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

        while (item->quantity > 0)
        // for (int i = 0; i < item->quantity; i++)
        {
            Candidate cand;
            Point t = item->poly.vertices()[0];
            Point l = freeSpace.front().outer_boundary().left_vertex()[0];
            auto trans = l - left;

            for (Point v : item->poly)
            {
                cand.poly.push_back(v + trans);
            }

            cand.index = item->index;
            cand.x_translation = trans.x();
            cand.y_translation = trans.y();

            // if (item->inners.empty())
            prob->addCandidate(cand, item->value);

            for (Polygon inner : item->inners)
            {
                Polygon innerCand;
                for (Point v : inner)
                {
                    innerCand.push_back(v + trans);
                }
                Candidate innerC;
                innerC.poly = innerCand;
                prob->addCandidate(innerC, 0);
            }

            // std::cout << " added";
            item->quantity--;

            if (item->quantity > 0)
            {
                Polygon_with_holes sum2 = CGAL::minkowski_sum_2(cand.poly, inverse);
                Pwh_list intR;
                for (Polygon_with_holes free : freeSpace)
                {
                    CGAL::difference(free, sum2, std::back_inserter(intR));
                }

                freeSpace = intR;

                if (freeSpace.empty())
                {
                    break;
                }
            }
        }
        // std::cout << std::endl;
        // break;
    }
    // std::cout << "doei\n";

    // prob->visualizeSolution();
    return SolveStatus::Feasible;
}
