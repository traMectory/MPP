/*#include "topos.h"

void toIPE(std::string path, Polygon boundary, std::vector<Polygon> polygons)
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

bool Topos::bestPlacement(std::vector<Polygon>& placed, Polygon& next, Polygon& leftContainer, Polygon& rightContainer, Point& translation) {
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
    std::list<Polygon_with_holes> noFitPlaced;

    //toIPE("test.ipe", next, { leftContainer, rightContainer });

    Polygon inverseNext;

    for (int i = 0; i < next.size(); i++)
    {
        Point p = next.vertices()[i];
        inverseNext.push_back(Point(-p.x(), -p.y()));
    }
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

    //Calculate No-Fit Polygon around previously placed pieces
    std::list<Point> possiblePositions;
    std::vector<Polygon_with_holes> possibleAreas;
    //std::vector<Polygon_with_holes>::const_iterator  it;

    //calculate No-Fit polygon for each separate part of placed pieces
    for (auto part : placed)
    {
        if (!part.is_empty()) {
            noFitPlaced.push_back(CGAL::minkowski_sum_2(part, inverseNext));
        }
    }

    //combine overlapping No-Fit polygons
    std::list<Polygon_with_holes>::iterator it = noFitPlaced.begin();
    bool noOverlap;
    while (it != noFitPlaced.end())
    {
        noOverlap = true;
        std::list<Polygon_with_holes>::iterator it2 = std::next(it);
        while (it2 != noFitPlaced.end())
        {
            Polygon_with_holes unionR;
            if (CGAL::join(*it, *it2, unionR)) {
                noFitPlaced.push_back(unionR);
                noFitPlaced.erase(it2);
                it = noFitPlaced.erase(it);
                noOverlap = false;
                break;
            }
            ++it2;
        }
        if (noOverlap)
            ++it;
    }

    //find parts of the No-Fit polygons that are inside container
    for (auto part : noFitPlaced)
    {
        CGAL::difference(outerBounds, part, std::back_inserter(possibleAreas));
    }

    //add vertices of remaining areas to possible positions
    if (!possibleAreas.empty()) {
        for (auto poly : possibleAreas) {
            for (auto v : poly.outer_boundary()) {
                possiblePositions.push_back(Point(v.x(), v.y()));
            }
        }
    }

    if (placed.empty())
    {
        for (auto v : outerBounds.vertices()) {
            possiblePositions.push_back(Point(v.x(), v.y()));
        }
    }


    if (possiblePositions.empty())
        return false;


    //find best position from all possible according to placement policy
    NT max = -1;
    Point bestPos;
    for (auto pos : possiblePositions)
    {
        NT eval = evalPlacement(placed, next, pos, leftContainer, rightContainer);
        if (eval > max) {
            max = eval;
            bestPos = pos;
        }
    }
    translation = bestPos;

    return true;
}

NT Topos::evalPlacement(std::vector<Polygon>& placed, Polygon& next, Point& position, Polygon& leftContainer, Polygon& rightContainer) {
    //evaluate quality of placing given piece at this position
    switch (placementPolicy)
    {
    case WASTE:
    {
        //use difference in size of new and old bounding boxes of placed pieces
        if (placed.size() <= 0 || next.is_empty())
            return 0;
        NT oldBB = 0;
        NT newBB = 0;
        for (auto pl : placed) {
            const Iso_rectangle plBB(pl.bbox());
            const Bbox nxBB = next.bbox();
            const Iso_rectangle combinedBB(Point(std::min(NT(plBB.xmin()), nxBB.xmin() + position.x()),
                std::min(NT(plBB.ymin()), nxBB.ymin() + position.y())),
                Point(std::max(NT(plBB.xmax()), nxBB.xmax() + position.x()),
                    std::max(NT(plBB.ymax()), nxBB.ymax() + position.y())));
            oldBB += plBB.area();
            newBB += combinedBB.area();
        }
        return 1.0 / (newBB - oldBB + 1);
    }
    case OVERLAP:
    {
        //use overlap between bounding boxes of next piece and the ones already placed
        if (placed.size() <= 0 || next.is_empty())
            return 0;
        NT sumOverlap = 0;
        for (auto pl : placed) {
            const Bbox nxBB = next.bbox();
            const Point nxMin(nxBB.xmin() + position.x(), nxBB.ymin() + position.y());
            const Point nxMax(nxBB.xmax() + position.x(), nxBB.ymax() + position.y());
            const auto result = CGAL::intersection(Iso_rectangle(pl.bbox()), Iso_rectangle(nxMin, nxMax));
            Iso_rectangle rectOverlap;
            if (result) {
                if (const Iso_rectangle* overlap = boost::get<Iso_rectangle>(&*result)) {
                    sumOverlap += overlap->area();
                }
                else {
                    throw std::invalid_argument("Cannot cast to Iso_rectangle");
                }
            }
        }
        return sumOverlap;
    }
    case DISTANCE:
    {
        //use (squared) distance between center of bounding boxes of placed pieces and next
        if (placed.size() <= 0 || next.is_empty())
            return 0;
        NT sumDist = 0;
        for (auto pl : placed) {
            const Bbox plBB = pl.bbox();
            const Bbox nxBB = next.bbox();
            const Point plCenter(plBB.xmin() + 0.5 * (plBB.xmax() - plBB.xmin()),
                plBB.ymin() + 0.5 * (plBB.ymax() - plBB.ymin()));
            const Point nxCenter(plBB.xmin() + position.x() + 0.5 * (plBB.xmax() - plBB.xmin()),
                plBB.ymin() + position.y() + 0.5 * (plBB.ymax() - plBB.ymin()));
            sumDist += CGAL::squared_distance(plCenter, nxCenter) + 1;
        }
        return 1.0 / (sumDist + 1);
    }
    default:
        throw std::invalid_argument("No valid placement policy chosen");
    }
}

SolveStatus Topos::solve(Problem* prob)
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
            if (!bestPlacement(placedPieces, item->poly, leftContainer, rightContainer, placement))
                continue;

            //check if tested piece has better evaluation than previous best
            NT eval = evalPlacement(placedPieces, item->poly, placement, leftContainer, rightContainer);
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

        bool connected = false;
        //update placed pieces, ignore holes that were created
        if (placedPieces.size() > 0) {
            for (size_t i = 0; i < placedPieces.size(); i++) {
                Polygon_with_holes newPlaced;
                if (CGAL::join(placedPieces[i], cand.poly, newPlaced) && newPlaced.outer_boundary().is_simple()) {
                    placedPieces[i] = newPlaced.outer_boundary();
                    connected = true;
                    break;
                }
            }
        }
        if (!connected) {
            placedPieces.push_back(cand.poly);
        }

        counter++;
        std::cout << counter << " pieces placed" << std::endl;
        if (counter >= 3000)
            break;
    }

    return SolveStatus::Feasible;

    /*
    //toIPE("test.ipe", container, { leftContainer, rightContainer });

    Polygon p1;

    //p1.push_back(Point(18195190, 24793702));
    //p1.push_back(Point(0, 5285760));
    //p1.push_back(Point(68, 188));
    //p1.push_back(Point(41320612, 0));

    p1.push_back(Point(18195190, 24793702));  p1.push_back(Point(0, 5285760));
    p1.push_back(Point(68, 188));   p1.push_back(Point(41320612, 0));

    Polygon p2;

    //p2.push_back(Point(0, 69791612));
    //p2.push_back(Point(8313820, 0));
    //p2.push_back(Point(36704770, 30439020));

    p2.push_back(Point(0, 69791612));  p2.push_back(Point(8313820, 0));
    p2.push_back(Point(36704770, 30439020));

    std::cout << "Polygon simple? " << p1.is_simple();
    std::cout << "Polygon simple? " << p2.is_simple();

    toIPE("test.ipe", container,  { p1, p2 });

    CGAL::minkowski_sum_2(p2, p1);

    return SolveStatus::Unsolved;*/
//}