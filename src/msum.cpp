#include "msum.h"

// Sort function based on area
bool compareItemsC(Item *i1, Item *i2)
{
    return i1->poly.area() > i2->poly.area();
};

/*
 * This function uses the Minkowski Sum to calculate if and where we can place an item, if a placement exists we simply put it as far left as possible.
 */
SolveStatus Msum::solve(Problem *prob)
{
    prob->addComment("Algorithm: msum");
    prob->addComment("Sorting: area dec");

    // Create the container which we use as obstacle to calculate the freespace around. The container has the shape of a C

    Polygon container = prob->getContainer();
    long wall = 10e6;

    Polygon target;
    target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymax() + wall));
    target.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymax() + wall));
    target.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymin() - wall));
    target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymin() - wall));

    auto v = container.right_vertex()[0];
    int n = 0;
    int i = container.size() - 1;
    bool adding = false;

    while (i > n)
    {
        if (!adding && container[i] == v)
        {
            adding = true;
            n -= (container.size() - i);
        }

        if (adding)
        {
            target.push_back(container[i < 0 ? i + container.size() : i]);
        }

        i--;
    }

    target.push_back(Point(container[i < 0 ? i + container.size() : i].x(), container[i < 0 ? i + container.size() : i].y() + 1));
    target.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymin() - wall + 1));

    // Get items and sort on size

    std::vector<Item *> copyItems = prob->getItems();
    std::sort(copyItems.begin(), copyItems.end(), compareItemsC);

    // Simply calculate total number of items (not same as num_items)
    int count = 0;
    int num_items = 0;
    for (Item *item : copyItems)
    {
        num_items += item->quantity;
    }
    std::cout << "total number of items: " << num_items << std::endl;

    // Algorithm is pretty simple and greedy. Look for a placement and place it, else no need to consider it.
    for (Item *item : copyItems)
    {
        if (item->quantity == 0)
            continue;

        count += item->quantity;

        // The next section calculates the freespace of our item. See slides on Minkowski Sum.
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
            if (hole.is_clockwise_oriented()) {
                hole.reverse_orientation();
            }
            // std::cout << hole.is_counterclockwise_oriented() << std::endl;
            freeSpace.push_back(Polygon_with_holes(hole));
        }

        // This the expensive part, as for every item we have already added we need to calculate
        //     the freespace and intersect it with the freespace calculated so far
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

        // If there are more items, we don't need to recalculate everything
        while (item->quantity > 0)
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
            cand.x_translation = (int) (trans.x().interval().pair().first + trans.x().interval().pair().second) / 2;
            cand.y_translation = (int) (trans.y().interval().pair().first + trans.y().interval().pair().second) / 2;

            prob->addCandidate(cand, item->value);

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
    }

    return SolveStatus::Feasible;
}
