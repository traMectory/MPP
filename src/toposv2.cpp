#include "toposv2.h"

void toIPE2(std::string path, Polygon boundary, std::vector<Polygon> polygons)
{
    std::ofstream o(path);

    // Find extreme coords of the graph (if you use CGAL you can use inbuild functions instead)

    NT xmin, xmax, ymin, ymax;
    for (Point p : boundary.vertices())
    {

        xmin = std::min(xmin, p.x());
        xmax = std::max(xmax, p.x());
        ymin = std::min(ymin, p.y());
        ymax = std::max(ymax, p.y());
    }
    for (Polygon poly : polygons)
    {
        for (Point p : poly.vertices())
        {

            xmin = std::min(xmin, p.x());
            xmax = std::max(xmax, p.x());
            ymin = std::min(ymin, p.y());
            ymax = std::max(ymax, p.y());
        }
    }
    NT scale = std::max(xmax - xmin, ymax - ymin);

    // Header of the IPE File
    o << "<?xml version=\"1.0\"?>\n";
    o << "<!DOCTYPE ipe SYSTEM \"ipe.dtd\">\n";
    o << "<ipe version=\"70218\" creator=\"Ipe 7.2.24\">\n";
    o << "<info created=\"D:20221020151441\" modified=\"D:20221020151441\"/>\n";
    o << "<ipestyle name=\"basic\">\n";
    o << "<symbol name=\"mark/disk(sx)\" transformations=\"translations\">\n";
    o << "<path fill=\"sym-stroke\">\n";
    o << "0.6 0 0 0.6 0 0 e\n";
    o << "</path>\n";
    o << "</symbol>\n";
    o << "<anglesize name=\"22.5 deg\" value=\"22.5\"/>\n";
    o << "<anglesize name=\"30 deg\" value=\"30\"/>\n";
    o << "<anglesize name=\"45 deg\" value=\"45\"/>\n";
    o << "<anglesize name=\"60 deg\" value=\"60\"/>\n";
    o << "<anglesize name=\"90 deg\" value=\"90\"/>\n";
    o << "<arrowsize name=\"large\" value=\"10\"/>\n";
    o << "<arrowsize name=\"small\" value=\"5\"/>\n";
    o << "<arrowsize name=\"tiny\" value=\"3\"/>\n";

    // If you want extra colors add here, see following lines for options
    o << "<color name=\"blue\" value=\"0 0 1\"/>\n";
    o << "<color name=\"gray\" value=\"0.745\"/>\n";
    o << "<color name=\"green\" value=\"0 1 0\"/>\n";
    o << "<color name=\"red\" value=\"1 0 0\"/>\n";
    o << "<pen name=\"heavier\" value=\"0.8\"/>\n";
    o << "<pen name=\"ultrafat\" value=\"2\"/>\n";

    o << "<gridsize name=\"16 pts (~6 mm)\" value=\"16\"/>\n";
    o << "<gridsize name=\"32 pts (~12 mm)\" value=\"32\"/>\n";
    o << "<gridsize name=\"4 pts\" value=\"4\"/>\n";
    o << "<gridsize name=\"8 pts (~3 mm)\" value=\"8\"/>\n";
    o << "<opacity name=\"10%\" value=\"0.1\"/>\n";
    o << "<opacity name=\"25%\" value=\"0.25\"/>\n";
    o << "<opacity name=\"50%\" value=\"0.5\"/>\n";
    o << "<opacity name=\"75%\" value=\"0.75\"/>\n";
    o << "<symbolsize name=\"large\" value=\"5\"/>\n";
    o << "<symbolsize name=\"small\" value=\"2\"/>\n";
    o << "<symbolsize name=\"tiny\" value=\"1.1\"/>\n";
    o << "<textsize name=\"huge\" value=\"\\huge\"/>\n";
    o << "<textsize name=\"large\" value=\"\\large\"/>\n";
    o << "<textsize name=\"small\" value=\"\\small\"/>\n";
    o << "<textsize name=\"tiny\" value=\"\tiny\"/>\n";
    o << "<tiling name=\"falling\" angle=\"-60\" step=\"4\" width=\"1\"/>\n";
    o << "<tiling name=\"rising\" angle=\"30\" step=\"4\" width=\"1\"/>\n";
    o << "</ipestyle>\n";
    o << "<page>\n";
    o << "<layer name=\"boundary\"/>\n";
    o << "<layer name=\"candidates\"/>\n";
    o << "<view layers=\"boundary candidates\" active=\"candidates\"/>\n";

    o << "<path layer=\"boundary\" stroke=\"black\">";

    bool first = true;

    for (Point v : boundary.vertices())
    {
        o << ((v.x() - xmin) * 560 / scale + 16) << " " << (v.y() * 560 / scale + 272) << " " << (first ? "m" : "l") << "\n";
        first = false;
    }
    o << "h\n</path>\n";

    for (auto poly : polygons)
    {
        o << "<path layer=\"candidates\" stroke=\"black\" fill=\"red\" opacity=\"25%\" stroke-opacity=\"opaque\">";

        bool first = true;

        for (Point v : poly.vertices())
        {
            o << ((v.x() - xmin) * 560 / scale + 16) << " " << (v.y() * 560 / scale + 272) << " " << (first ? "m" : "l") << "\n";
            first = false;
        }
        o << "h\n</path>\n";
    }

    o << "</page>\n";
    o << "</ipe>\n";

    o.close();

    std::string systemCom = "ipe " + path;

    int systemRet = system(systemCom.c_str());

    if (systemRet == -1)
    {
        printf("Could not open IPE");
    }
}

bool Toposv2::bestPlacement(Polygon& next, Polygon& leftContainer, Polygon& rightContainer, Point& translation) {
    /// <summary>
    /// finds the best placement for next piece, given current configuration
    /// </summary>
    /// <param name="placed">boundary of previously placed polygons</param>
    /// <param name="next">polygon to be placed</param>
    /// <param name="leftContainer">left portion of container</param>
    /// <param name="rightContainer">right portion</param>
    /// <param name="translation">resulting position of best placement</param>
    /// <returns>wheter or not a valid placement was found</returns>
    Polygon_with_holes noFitLeft;
    Polygon_with_holes noFitRight;

    //toIPE("test.ipe", next, { leftContainer, rightContainer });

    Polygon inverseNext;

    for (int i = 0; i < next.size(); i++)
    {
        Point p = next.vertices()[i];
        inverseNext.push_back(Point(-p.x(), -p.y()));
    }
    //toIPE2("test.ipe", leftContainer, { next });
    //Calculate polygon in which the new piece can be placed
    noFitLeft = CGAL::minkowski_sum_2(leftContainer, inverseNext);
    //toIPE("test.ipe", noFitLeft.outer_boundary(), { leftContainer, next });
    noFitRight = CGAL::minkowski_sum_2(rightContainer, inverseNext);
    //toIPE("test.ipe", noFitRight.outer_boundary(), { rightContainer, next });

    Polygon_with_holes noFitContainer;
    CGAL::join(noFitLeft, noFitRight, noFitContainer);


    if (noFitContainer.number_of_holes() == 0)
        //no space for polygon inside container
        return false;

    Polygon outerBounds = noFitContainer.holes().at(0);
    outerBounds.reverse_orientation();

    //toIPE("test.ipe", noFitLeft.outer_boundary(), { outerBounds, next });

    std::vector<Point> possiblePositions;

    //add vertices of remaining areas to possible positions
    for (auto v : outerBounds) {
        possiblePositions.push_back(Point(v.x(), v.y()));
    }



    if (possiblePositions.empty())
        return false;


    //find best position from all possible according to placement policy
    NT max = -1;
    Point bestPos;
    for (auto pos : possiblePositions)
    {
        NT eval = evalPlacement( next, pos, leftContainer, rightContainer);
        if (eval > max) {
            max = eval;
            bestPos = pos;
        }
    }
    translation = bestPos;

    return true;
}

NT Toposv2::evalPlacement(Polygon& next, Point& position, Polygon& leftContainer, Polygon& rightContainer) {
    //evaluate quality of placing given piece at this position
    switch (placementPolicy)
    {
    case WASTE:
    {
        //use difference in size of new and old bounding boxes of placed pieces
        return 1;
    }
    case OVERLAP:
    {
        //use overlap between bounding boxes of next piece and the ones already placed
        return 1;
    }
    case DISTANCE:
    {
        //use (squared) distance between center of bounding boxes of placed pieces and next

        return 1;
    }
    default:
        throw std::invalid_argument("No valid placement policy chosen");
    }
}

SolveStatus Toposv2::solve(Problem* prob)
{

    //outer boundaries of previously placed pieces, each element is one connected part to keep polygons simple
    std::vector<Polygon> placedPieces;

    Polygon container = prob->getContainer();
    int wall = 10e6;


    //split container into left and right portion

    auto bottom = container.bottom_vertex();
    auto top = container.top_vertex();

    Polygon leftContainer;
    Polygon rightContainer;

    //build left container

    leftContainer.push_back(Point(bottom->x(), bottom->y() - wall));

    for (auto v = bottom; v != top; v = v == container.begin() ? container.end() - 1 : v - 1) {
        leftContainer.push_back(Point(v->x(), v->y()));
    }
    leftContainer.push_back(Point(top->x(), top->y()));
    leftContainer.push_back(Point(top->x(), top->y() + wall));
    leftContainer.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymax() + wall));
    leftContainer.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymin() - wall));

    //build right container

    rightContainer.push_back(Point(top->x(), top->y() + wall));

    for (auto v = top; v != bottom; v = v == container.begin() ? container.end() - 1 : v - 1) {
        rightContainer.push_back(Point(v->x(), v->y()));
    }
    rightContainer.push_back(Point(bottom->x(), bottom->y()));
    rightContainer.push_back(Point(bottom->x(), bottom->y() - wall));
    rightContainer.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymin() - wall));
    rightContainer.push_back(Point(container.bbox().xmax() + wall, container.bbox().ymax() + wall));


    std::vector<Item*> copyItems = prob->getItems();
    std::list<Item*> itemList(copyItems.begin(), copyItems.end());

    int counter = 0;
    while (itemList.size() > 0) {

        //iterate all pieces and find best candidate for next placement
        NT max = -1;
        Item* bestPiece;
        Point bestPiecePlacement;
        for (auto item : itemList)
        {
            //find best placement for current piece
            Point placement;
            if (!bestPlacement(item->poly, leftContainer, rightContainer, placement))
                continue;

            //check if tested piece has better evaluation than previous best
            NT eval = evalPlacement(item->poly, placement, leftContainer, rightContainer);
            if (eval > max) {
                max = eval;
                bestPiece = item;
                bestPiecePlacement = placement;
            }

            //for now use first possible piece
            break;
        }

        //no valid placement found
        if (bestPiece == nullptr)
            break;

        //Place chosen piece
        Candidate cand;

        for (Point v : bestPiece->poly)
        {
            Point p(v.x() + bestPiecePlacement.x(), v.y() + bestPiecePlacement.y());
            cand.poly.push_back(p);
        }

        cand.index = bestPiece->index;
        cand.x_translation = bestPiecePlacement.x();
        cand.y_translation = bestPiecePlacement.y();

        prob->addCandidate(cand, bestPiece->value);

        bestPiece->quantity--;
        if (bestPiece->quantity <= 0)
            itemList.remove(bestPiece);

        auto it = CGAL::Circulator_from_iterator(leftContainer.begin(), leftContainer.end(), leftContainer.bottom_vertex());
        ++it;
        bool leftInsert = false;
        //update container with new piece
        
        Point previous = *it;
        //find start vertex to insert right boundary into left container
        if (true || CGAL::do_intersect(leftContainer, cand.poly)) {
            leftInsert = true;
            while (it->x() > leftContainer.left_vertex()->x() || it->y() > leftContainer.bottom_vertex()->y())
            {
                //move up boundary until height of new piece is reached
                if (it->y() < cand.poly.bottom_vertex()->y())
                {
                    previous = *it;
                    ++it;
                }
                else{
                    //add piece's right boundary to container
                    auto placedIt = CGAL::Circulator_from_iterator(cand.poly.begin(), cand.poly.end(), cand.poly.bottom_vertex());
                    auto endIter = cand.poly.top_vertex();

                    if (placedIt->y() < it->y()) {
                        //create new vertex to the left of lowest vertex of new piece
                        NT xIntersect = previous.x() + (it->x() - previous.x()) * (placedIt->y() - previous.y()) / (it->y() - previous.y());
                        //only if new piece is not already on boundary
                        if (xIntersect != placedIt->x())
                        {
                            it = CGAL::Circulator_from_iterator(leftContainer.begin(), leftContainer.end(), leftContainer.insert(it.current_iterator(), Point(xIntersect, placedIt->y())));
                            ++it;
                        }
                    }
                    else
                    {
                        if (placedIt->x() == it->x())
                            ++placedIt;
                        else
                            ++it;
                    }
                    previous = *placedIt;
                    do {
                        if (previous.y() > placedIt->y()) {
                            --placedIt;
                            --placedIt;
                            Point& before = *placedIt;
                            ++placedIt;
                            ++placedIt;
                            
                            //remove non-y-monotone parts
                            if ((previous.x() - before.x()) * (placedIt->y() - before.y()) - (previous.y() - before.y()) * (placedIt->x() - before.x()) < 0) {
                                --it;
                                while (it->y() > placedIt->y()) {
                                    it = CGAL::Circulator_from_iterator(leftContainer.begin(), leftContainer.end(), leftContainer.erase(it.current_iterator()));
                                    --it;
                                }
                                ++it;
                                it = CGAL::Circulator_from_iterator(leftContainer.begin(), leftContainer.end(), leftContainer.insert(it.current_iterator(), Point(placedIt->x(), placedIt->y())));
                                ++it;
                            }
                        }
                        else {
                            it = CGAL::Circulator_from_iterator(leftContainer.begin(), leftContainer.end(), leftContainer.insert(it.current_iterator(), Point(placedIt->x(), placedIt->y())));
                            ++it;
                        }
                        previous = *placedIt;
                        ++placedIt;
                    } while (previous.y() < endIter->y());

                    while (it->y() < endIter->y()) {
                        previous = *it;
                        it = CGAL::Circulator_from_iterator(leftContainer.begin(), leftContainer.end(), leftContainer.erase(it.current_iterator()));
                    }
                    if (it->y() == endIter->y()) {
                        if (it->x() == endIter->x()) {
                            leftContainer.erase(it.current_iterator());
                        }
                        else {
                            ++it;
                            if (it->y() == endIter->y()) {
                                --it;
                                leftContainer.erase(it.current_iterator());
                            }
                        }
                    }
                    else {
                        NT xIntersect = previous.x() + (it->x() - previous.x()) * (endIter->y() - previous.y()) / (it->y() - previous.y());
                        if (xIntersect > endIter->x())
                            leftContainer.insert(it.current_iterator(), Point(xIntersect, endIter->y()));
                    }
                    //toIPE2("test.ipe", leftContainer, { cand.poly });
                    break;
                }
            }
        }

        //toIPE2("test.ipe", leftContainer, { cand.poly });

        counter++;
        std::cout << counter << " pieces placed" << std::endl;
        
        if (counter >= 100)
            break;
    }

    return SolveStatus::Feasible;
}