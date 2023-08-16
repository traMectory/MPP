#include "msum.h"

bool compareItems(Item i1, Item i2)
{
    return i1.poly.area() > i2.poly.area();
};

SolveStatus Msum::solve(Problem *prob)
{
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

    Polygon outer_target;
    outer_target.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymin() - wall));
    outer_target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymin() - wall));
    outer_target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymax() + wall));
    outer_target.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymax() + wall));

    Polygon_with_holes target(outer_target);
    target.add_hole(container);

    std::vector<Item> copyItems = prob->getItems();

    std::sort(copyItems.begin(), copyItems.end(), compareItems);
    CGAL::Polygon_vertical_decomposition_2<K> ssab_decomp;

    int count = 0;
    int num_items = 0;
    for (Item item : copyItems) {
        num_items += item.quantity;
    }

    for (Item item : copyItems)
    {
        std::cout << "(" << count<<"/" << num_items << ")" << " q: " << item.quantity << ", v: " << item.value << ", v/a: " << (item.value / item.poly.area());
        count += item.quantity;
        for (int i = 0; i < item.quantity; i++)
        {

            Polygon inverse;
            Point left = item.poly.left_vertex()[0];

            for (int i = 0; i < item.poly.size(); i++)
            {
                Point p = item.poly.vertices()[i];
                auto newP = p - left;
                inverse.push_back(Point(-newP.x(), -newP.y()));
            }

            Polygon_with_holes sum = CGAL::minkowski_sum_2(target, inverse, ssab_decomp);

            if (sum.has_holes())
            {
                Pwh_list freeSpace;
                for (Polygon hole : sum.holes())
                {
                    freeSpace.push_back(Polygon_with_holes(hole));
                }

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

                Polygon cand;
                Point t = item.poly.vertices()[0];
                Point l = freeSpace.front().outer_boundary().left_vertex()[0];
                auto trans = l - left;

                for (Point v : item.poly)
                {
                    cand.push_back(v + trans);
                }

                prob->addCandidate(cand, item.value);

                std::cout << " added";
            }
        }
        std::cout << std::endl;
        // break;
    }
    std::cout << "doei\n";

    prob->visualizeSolution();
    return SolveStatus::Feasible;
}
