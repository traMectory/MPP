#include "toposv2.h"

void toIPE2(std::string path, Polygon boundary, std::vector<Polygon> polygons, std::vector<Point> points)
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
    o << "<layer name=\"points\"/>\n";
    o << "<view layers=\"boundary candidates points\" active=\"candidates\"/>\n";

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

    for (auto p : points)
    {
        o << "<path layer=\"points\" fill=\"blue\" opacity=\"75%\" stroke-opacity=\"opaque\">\n";
        o << "0.5 0 0 0.5 " << ((p.x() - xmin) * 560 / scale + 16) << " " << (p.y() * 560 / scale + 272) << " e\n";
        o << "\n</path>\n";
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

bool Toposv2::bestPlacement(Polygon& next, Polygon& container,std::vector<Iso_rectangle>& bBoxes, Iso_rectangle& placedBBox , Point& translation, NT& bestEval) {
    /// <summary>
    /// finds the best placement for next piece, given current configuration
    /// </summary>
    /// <param name="placed">boundary of previously placed polygons</param>
    /// <param name="next">polygon to be placed</param>
    /// <param name="leftContainer">left portion of container</param>
    /// <param name="rightContainer">right portion</param>
    /// <param name="translation">resulting position of best placement</param>
    /// <returns>wheter or not a valid placement was found</returns>
    Polygon_with_holes noFit;
    
    //toIPE("test.ipe", next, { leftContainer, rightContainer });
    
    Polygon inverseNext;

    for (int i = 0; i < next.size(); i++)
    {
        Point p = next.vertices()[i];
        inverseNext.push_back(Point(-p.x(), -p.y()));
    }
    //toIPE2("test.ipe", leftContainer, { next });
    //Calculate polygon in which the new piece can be placed
    noFit = CGAL::minkowski_sum_2(container, inverseNext);
    //toIPE("test.ipe", noFitLeft.outer_boundary(), { leftContainer, next });

    if (noFit.number_of_holes() == 0)
        //no space for polygon inside container
        return false;

    Polygon outerBounds = noFit.holes().at(0);
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
    bestEval = 10e40;
    Point bestPos;
    for (auto pos : possiblePositions)
    {
        NT eval = evalPlacement(next, pos, container, bBoxes, placedBBox);
        if (eval < bestEval) {
            bestEval = eval;
            bestPos = pos;
        }
    }
    translation = bestPos;

    return true;
}

NT Toposv2::evalPlacement(Polygon& next, Point& position, Polygon& container, std::vector<Iso_rectangle>& bBoxes, Iso_rectangle& placedBBox) {
    //evaluate quality of placing given piece at this position
    //return position.y() + position.x() * 0.000001;
    
    NT eval = 0;
    const Iso_rectangle newBBox = Iso_rectangle(Point(next.bottom_vertex()->x() + position.x(), next.bottom_vertex()->y() + position.y()),
        Point(next.top_vertex()->x() + position.x(), next.top_vertex()->y() + position.y()));

    if (bBoxes.size() > 0) {
        //calculate WASTE - difference in size of new and old bounding boxes of placed pieces
        Iso_rectangle newPlaced = Iso_rectangle(Point(std::min(newBBox.xmin(), placedBBox.xmin()), std::min(newBBox.ymin(), placedBBox.ymin())),
            Point(std::max(newBBox.xmax(), placedBBox.xmax()), std::max(newBBox.ymax(), placedBBox.ymax())));
        eval += newPlaced.area() - placedBBox.area();


        //calculate DISTANCE - (squared) distance between center of bounding boxes of placed pieces and next
        const Point placedCenter(0.5 * (placedBBox.xmin() + placedBBox.xmax()), 0.5 * (placedBBox.ymin() + placedBBox.ymax()));
        const Point newCenter(0.5 * (newBBox.xmin() + newBBox.xmax()), 0.5 * (newBBox.ymin() + newBBox.ymax()));
        //eval += CGAL::approximate_sqrt(CGAL::squared_distance(placedCenter, newCenter));
    }
    
    //calculate OVERLAP - overlap between bounding boxes of next piece and the ones already placed
    for (auto bbox : bBoxes) {
        const auto result = CGAL::intersection(bbox, newBBox);
        if (result) {
            if (const Iso_rectangle* r = boost::get<Iso_rectangle>(&*result)) {
                eval -= r->area();
            }
        }
    }

    //encourage bottom-left placement
    eval += position.y() + position.x();
    
    return eval;
}

void Toposv2::addNewPieceExact(Candidate& cand, Polygon& container) {
    //Polygon_with_holes res;
    //CGAL::join(container, cand.poly, res);
    //container = res.outer_boundary();
    //return;

    //add the newly placed polygon to container
    auto forward = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.right_vertex());
    forward += 2;
    if (forward->y() == container.top_vertex()->y())
        ++forward;
    //update container with new piece
    NT xmin = cand.poly.left_vertex()->x();
    NT xmax = cand.poly.right_vertex()->x();
    NT ymin = cand.poly.bottom_vertex()->y();
    NT ymax = cand.poly.top_vertex()->y();
    //construct bounding sphere around polygon
    //Point polyCenter((xmin + xmax) / 2, (ymin + ymax) / 2);
    //NT rSq = CGAL::sqrt(CGAL::squared_distance(polyCenter, Point(xmin, ymin)));

    Point previous = *forward;
    //find start vertex to insert new polygon

    //insert into leftContainer
    auto polyStart = cand.poly.begin();
    auto polyEnd = cand.poly.end();
    --polyEnd;

    if (DEBUG)
        toIPE2("test.ipe", container, { cand.poly }, {*container.begin()});

    //find first intersection point bottom-up
    bool intersectFound = false;
    while (!intersectFound) {
        if (true){
            Point prevCand = *polyEnd;
            bool edgeContact = false;
            for (auto v = cand.poly.begin(); v != cand.poly.end(); ++v) {
                if (!intersectFound) {
                    if (*v == *forward) {
                        //point of container touches point of polygon
                        intersectFound = true;
                        polyStart = v;
                        if (polyStart == polyEnd)
                            polyStart = cand.poly.begin();
                        ++polyStart;
                    }
                    else if (CGAL::collinear(prevCand, *v, *forward) && (prevCand < *forward && *forward < *v || prevCand > *forward && *forward > *v)) {
                        //point of container touches edge of polygon
                        intersectFound = true;
                        polyStart = v;
                    }
                }
                if (CGAL::collinear(previous, *forward, *v) && (previous < *v && *v < *forward || previous > *v && *v > *forward)) {
                    //point(s) of polygon touch edge of container
                    if (!edgeContact || CGAL::squared_distance(*v, *forward) > CGAL::squared_distance(*polyStart, *forward)) {
                        polyStart = v;
                        edgeContact = true;
                        intersectFound = true;
                    }
                }
                prevCand = *v;
            }
            if (edgeContact)
                --forward;
            if (intersectFound)
                break;
        }
        previous = *forward;
        ++forward;
    }

    auto backward = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.left_vertex());
    backward -= 2;
    if (backward->y() == container.top_vertex()->y())
        --backward;
    //find first intersection point top-down
    intersectFound = false;
    while (!intersectFound) {
        if (true){
            Point prevCand = *cand.poly.begin();
            bool edgeContact = false;
            for (auto v = polyEnd;;--v) {
                if (!intersectFound) {
                    if (*v == *backward) {
                        //point of container touches point of polygon
                        intersectFound = true;
                        polyEnd = v;
                        if (polyEnd == cand.poly.begin())
                            polyEnd = cand.poly.end();
                        --polyEnd;
                    }
                    else if (CGAL::collinear(prevCand, *v, *backward) && (prevCand < *backward && *backward < *v || prevCand > *backward && *backward > *v)) {
                        //point of container touches edge of polygon
                        intersectFound = true;
                        polyEnd = v;
                    }
                }
                if (CGAL::collinear(previous, *backward, *v) && (previous < *v && *v < *backward || previous > *v && *v > *backward)) {
                    //point(s) of polygon touch edge of container
                    if (!edgeContact || CGAL::squared_distance(*v, *backward) > CGAL::squared_distance(*polyEnd, *backward)) {
                        polyEnd = v;
                        edgeContact = true;
                        intersectFound = true;
                    }
                }
                prevCand = *v;
                if (v == cand.poly.begin())
                    break;
            }
            if (edgeContact)
                ++backward;
            if (intersectFound)
                break;
        }
        previous = *backward;
        --backward;
    }

    //erase part of old container that will be replaced
    if(backward != forward)
        --backward;

    while (backward != forward) {
        auto fwd = forward.current_iterator() - container.begin();
        if (backward.current_iterator() < forward.current_iterator())
            fwd -= 1;
        backward = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.erase(backward.current_iterator()));
        forward = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.begin()) + fwd;
        --backward;
    }
    ++backward;

    //insert part of polygon between first and last intersection
    auto polyIter = CGAL::Circulator_from_iterator(cand.poly.begin(), cand.poly.end(), polyStart);
    while (polyIter.current_iterator() != polyEnd) {
        backward = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.insert(backward.current_iterator(), *polyIter));
        //toIPE2("test.ipe", leftContainer, { cand.poly });
        ++backward;
        ++polyIter;
    }
    container.insert(backward.current_iterator(), *polyEnd);
    //toIPE2("test.ipe", leftContainer, { cand.poly });
}

void Toposv2::addNewPieceApprox(Candidate& cand, Polygon& container) {
    //Polygon_with_holes res;
    //CGAL::join(container, cand.poly, res);
    //container = res.outer_boundary();
    //return;

    NT EPSILON = 10e8;

    //add the newly placed polygon to container
    auto forward = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.right_vertex());
    forward += 2;
    if (forward->y() == container.top_vertex()->y())
        ++forward;
    //update container with new piece
    NT xmin = cand.poly.left_vertex()->x();
    NT xmax = cand.poly.right_vertex()->x();
    NT ymin = cand.poly.bottom_vertex()->y();
    NT ymax = cand.poly.top_vertex()->y();
    //construct bounding sphere around polygon
    //Point polyCenter((xmin + xmax) / 2, (ymin + ymax) / 2);
    //NT rSq = CGAL::sqrt(CGAL::squared_distance(polyCenter, Point(xmin, ymin)));

    Point previous = *(forward-1);
    //find start vertex to insert new polygon

    //insert into leftContainer
    auto polyStart = cand.poly.begin();
    auto polyEnd = cand.poly.end();
    --polyEnd;

    if (DEBUG)
        toIPE2("test.ipe", container, { cand.poly }, { *container.begin() });

    //find first intersection point bottom-up
    bool intersectFound = false;
    while (!intersectFound) {
        if (true) {
            Point prevCand = *polyEnd;
            bool edgeContact = false;
            for (auto v = cand.poly.begin(); v != cand.poly.end(); ++v) {
                if (!intersectFound) {
                    if (*v == *forward) {
                        //point of container touches point of polygon
                        intersectFound = true;
                        polyStart = v;
                        if (polyStart == polyEnd)
                            polyStart = cand.poly.begin();
                        ++polyStart;
                    }
                    else if (CGAL::squared_distance(*forward, Line(prevCand, *v)) <= EPSILON && CGAL::angle(*forward, prevCand, *v) == CGAL::ACUTE && CGAL::angle(*forward, *v, prevCand) == CGAL::ACUTE) {
                        //point of container touches edge of polygon
                        intersectFound = true;
                        polyStart = v;
                    }
                }
                if (CGAL::squared_distance(*v, Line(previous, *forward)) <= EPSILON && CGAL::angle(*v, previous, *forward) == CGAL::ACUTE && CGAL::angle(*v, *forward, previous) == CGAL::ACUTE) {
                    //point(s) of polygon touch edge of container
                    if (!edgeContact || CGAL::squared_distance(*v, *forward) > CGAL::squared_distance(*polyStart, *forward)) {
                        polyStart = v;
                        edgeContact = true;
                        intersectFound = true;
                    }
                }
                prevCand = *v;
            }
            if (edgeContact)
                --forward;
            if (intersectFound)
                break;
        }
        previous = *forward;
        ++forward;
    }

    auto backward = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.left_vertex());
    backward -= 2;
    if (backward->y() == container.top_vertex()->y())
        --backward;
    previous = *(backward + 1);
    //find first intersection point top-down
    intersectFound = false;
    while (!intersectFound) {
        if (true) {
            Point prevCand = *cand.poly.begin();
            bool edgeContact = false;
            for (auto v = polyEnd;; --v) {
                if (!intersectFound) {
                    Line edge(prevCand, *v);
                    if (*v == *backward) {
                        //point of container touches point of polygon
                        intersectFound = true;
                        polyEnd = v;
                        if (polyEnd == cand.poly.begin())
                            polyEnd = cand.poly.end();
                        --polyEnd;
                    }
                    else if (CGAL::squared_distance(*backward, edge) <= EPSILON && CGAL::angle(*backward, prevCand, *v) == CGAL::ACUTE && CGAL::angle(*backward, *v, prevCand) == CGAL::ACUTE) {
                        //point of container touches edge of polygon
                        intersectFound = true;
                        polyEnd = v;
                    }
                }
                if (CGAL::squared_distance(*v, Line(previous, *backward)) <= EPSILON && CGAL::angle(*v, previous, *backward) == CGAL::ACUTE && CGAL::angle(*v, *backward, previous) == CGAL::ACUTE) {
                    //point(s) of polygon touch edge of container
                    if (!edgeContact || CGAL::squared_distance(*v, *backward) > CGAL::squared_distance(*polyEnd, *backward)) {
                        polyEnd = v;
                        edgeContact = true;
                        intersectFound = true;
                    }
                }
                prevCand = *v;
                if (v == cand.poly.begin())
                    break;
            }
            if (edgeContact)
                ++backward;
            if (intersectFound)
                break;
        }
        previous = *backward;
        --backward;
    }

    //erase part of old container that will be replaced
    if (backward != forward)
        --backward;

    while (backward != forward) {
        auto fwd = forward.current_iterator() - container.begin();
        if (backward.current_iterator() < forward.current_iterator())
            fwd -= 1;
        backward = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.erase(backward.current_iterator()));
        forward = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.begin()) + fwd;
        --backward;
    }
    ++backward;

    //insert part of polygon between first and last intersection
    auto polyIter = CGAL::Circulator_from_iterator(cand.poly.begin(), cand.poly.end(), polyStart);
    while (polyIter.current_iterator() != polyEnd) {
        backward = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.insert(backward.current_iterator(), *polyIter));
        //toIPE2("test.ipe", leftContainer, { cand.poly });
        ++backward;
        ++polyIter;
    }
    container.insert(backward.current_iterator(), *polyEnd);
    //toIPE2("test.ipe", leftContainer, { cand.poly });
}

void Toposv2::addNewPieceRightBounds(Candidate& cand, Polygon& leftContainer, Polygon& rightContainer) {
    //add the newly placed piece to container by extending its lowest and highest point to -inf

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
            else {
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
                            while (it->y() == endIter->y()) {
                                it = CGAL::Circulator_from_iterator(leftContainer.begin(), leftContainer.end(), leftContainer.erase(it.current_iterator()));
                                --it;
                            }
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
}

SolveStatus Toposv2::solve(Problem* prob)
{
    //outer boundaries of previously placed pieces, each element is one connected part to keep polygons simple
    std::vector<Polygon> placedPieces;

    Polygon container = prob->getContainer();
    int wall = 10e6;


    //build new polygon as outside of container

    auto bottom = container.bottom_vertex();
    auto top = container.top_vertex();

    container.reverse_orientation();
    auto containerStart = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.top_vertex());
    auto containerEnd = containerStart - 1;

    NT dist = CGAL::squared_distance(*containerStart, *containerEnd);
    containerEnd = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.insert(containerStart.current_iterator(),
        Point(containerStart->x() + (containerEnd->x() - containerStart->x()) / dist, containerStart->y() + (containerEnd->y() - containerStart->y()) / dist)));
    containerStart = containerEnd + 1;

    auto it = container.insert(containerStart.current_iterator(), Point(containerStart->x(), containerStart->y() + wall));
    it = container.insert(it, Point(container.right_vertex()->x() + wall, it->y()));
    it = container.insert(it, Point(it->x(), container.bottom_vertex()->y() - wall));
    it = container.insert(it, Point(container.left_vertex()->x() - wall, it->y()));
    it = container.insert(it, Point(it->x(), container.top_vertex()->y()));

    containerEnd = CGAL::Circulator_from_iterator(container.begin(), container.end(), it) - 1;
    containerEnd = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.insert(it, Point(containerEnd->x(), it->y()))) -1;
    containerStart = containerEnd + 7;

    std::vector<Item*> copyItems = prob->getItems();
    std::list<Item*> itemList(copyItems.begin(), copyItems.end());

    std::vector<Iso_rectangle> bBoxes;
    Iso_rectangle placedBBox;

    int counter = 0;
    while (itemList.size() > 0) {

        //iterate all pieces and find best candidate for next placement
        NT min = 10e40;
        Item* bestPiece = nullptr;
        Point bestPiecePlacement;
        std::list<Item*>::iterator item = itemList.begin();
        while (item != itemList.end())
        {
            //find best placement for current piece
            Point placement;
            NT eval;
            if (!bestPlacement((*item)->poly, container, bBoxes, placedBBox, placement, eval)) {
                item = itemList.erase(item);
                if(VERBOSE)
                    std::cout << "Could not find placement!\n";
                continue;
            }

            //check if tested piece has better evaluation than previous best
            if (eval < min) {
                min = eval;
                bestPiece = *item;
                bestPiecePlacement = placement;
            }

            ++item;
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

        switch (joinMode)
        {
        case joinType::EXACT:
            addNewPieceExact(cand, container);
            break;
        case joinType::APPROXIMATE:
            addNewPieceApprox(cand, container);
            break;
        default:
            throw std::invalid_argument("No valid join operation");
        }

        Iso_rectangle newBBox = Iso_rectangle(Point(cand.poly.bottom_vertex()->x(), cand.poly.bottom_vertex()->y()), Point(cand.poly.top_vertex()->x(), cand.poly.top_vertex()->y()));
        //add polygon's BBox to existing ones
        if (bBoxes.size() == 0)
            placedBBox = newBBox;
        else
        {
            placedBBox = Iso_rectangle(Point(std::min(newBBox.xmin(), placedBBox.xmin()), std::min(newBBox.ymin(), placedBBox.ymin())),
                Point(std::max(newBBox.xmax(), placedBBox.xmax()), std::max(newBBox.ymax(), placedBBox.ymax())));
        }
        bBoxes.push_back(newBBox);


        cand.index = bestPiece->index;
        cand.x_translation = bestPiecePlacement.x();
        cand.y_translation = bestPiecePlacement.y();

        prob->addCandidate(cand, bestPiece->value);

        bestPiece->quantity--;
        if (bestPiece->quantity <= 0)
            itemList.remove(bestPiece);

        

        //toIPE2("test.ipe", leftContainer, { cand.poly });

        counter++;
        if(VERBOSE)
            std::cout << counter << " pieces placed" << std::endl;

        if (DEBUG)
            toIPE2("test.ipe", container, { cand.poly }, {});

    }
    return SolveStatus::Feasible;
}