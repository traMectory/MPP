#include "topos.h"

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

bool Topos::bestPlacement(Polygon &placed, Polygon &next, Polygon &leftContainer, Polygon &rightContainer, Point &translation) {
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
    Polygon_with_holes noFitPlaced;

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


    if(noFitContainer.number_of_holes() == 0)
        //no space for polygon inside container
        return false;
    
    Polygon outerBounds = noFitContainer.holes().at(0);

    //toIPE("test.ipe", noFitLeft.outer_boundary(), { outerBounds, next });

    //Calculate No-Fit Polygon around previously placed pieces
    std::vector<Point> possiblePositions;
    std::vector<Polygon_with_holes> possibleAreas;
    if (!placed.is_empty()) {
        noFitPlaced = CGAL::minkowski_sum_2(placed, inverseNext);
        CGAL::difference(outerBounds, noFitPlaced, std::back_inserter(possibleAreas));
        if (!possibleAreas.empty()) {
            for (auto poly : possibleAreas) {
                for (auto v : poly.outer_boundary()) {
                    possiblePositions.push_back(Point(v.x(), v.y()));
                }
            }
        }
    }
    else
    {
        for (auto v : outerBounds.vertices()) {
            possiblePositions.push_back(Point(v.x(), v.y()));
        }
    }


    if (possiblePositions.empty())
        return false;
    //TODO find best position from all possible, for now first one found is used
    translation = possiblePositions[0];
    return true;
}

float Topos::evalPlacement(Polygon& placed, Polygon& next, Point &position ,Polygon& leftContainer, Polygon& rightContainer) {
    //TODO evaluate quality of placing given piece at this position
    return 1.0;
}

SolveStatus Topos::solve(Problem* prob)
{
    CGAL::set_warning_behaviour(CGAL::CONTINUE);

    Polygon placedPieces; //outer boundary of previously placed pieces

	Polygon container = prob->getContainer();
	int wall = 10e6;


    //split container into left and right portion

    auto bottom = container.bottom_vertex();
    auto top = container.top_vertex();

    Polygon leftContainer;
    Polygon rightContainer;

    //build left container

    leftContainer.push_back(Point(bottom->x(), bottom->y() - wall));

    for (auto v = bottom; v != top; v = v==container.begin() ? container.end()-1 : v-1) {
        leftContainer.push_back(Point(v->x(), v->y()));
    }
    leftContainer.push_back(Point(top->x(), top->y()));
    leftContainer.push_back(Point(top->x(), top->y() + wall));
    leftContainer.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymax() + wall));
    leftContainer.push_back(Point(container.bbox().xmin() - wall, container.bbox().ymin() - wall));

    //build right container

    rightContainer.push_back(Point(top->x(), top->y() + wall));

    for (auto v = top; v != bottom; v = v == container.begin() ? container.end()-1 : v-1) {
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
        float max = .0;
        Item* bestPiece;
        Point bestPiecePlacement;
        for (auto item : itemList)
        {
            //find best placement for current piece
            Point placement;
            if (!bestPlacement(placedPieces, item->poly, leftContainer, rightContainer, placement))
                continue;

            //check if tested piece has better evaluation than previous best
            float eval = evalPlacement(placedPieces, item->poly, placement, leftContainer, rightContainer);
            if (eval > max) {
                max = eval;
                bestPiece = item;
                bestPiecePlacement = placement;
            }

            //for now use first possible placement
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

        //update placed pieces, ignore holes that were created
        if (placedPieces.size() > 0) {
            Polygon_with_holes newPlaced;
            CGAL::join(placedPieces, cand.poly, newPlaced);
            placedPieces = newPlaced.outer_boundary();
        }
        else {
            placedPieces = cand.poly;
        }

        counter++;
        if (counter > 10)
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
}