#include "incrementalnofitsolver.h"

void pathsToIPE(std::string path, Path boundary, std::vector<Path> polygons)
{
	std::ofstream o(path);

	// Find extreme coords of the graph (if you use CGAL you can use inbuild functions instead)

	int64_t xmin=999999999, xmax=-999999999, ymin = 9999999999, ymax=-999999999;
	for (auto p : boundary)
	{

		xmin = std::min(xmin, p.x);
		xmax = std::max(xmax, p.x);
		ymin = std::min(ymin, p.y);
		ymax = std::max(ymax, p.y);
	}
	for (auto poly : polygons)
	{
		for (auto p : poly)
		{

			xmin = std::min(xmin, p.x);
			xmax = std::max(xmax, p.x);
			ymin = std::min(ymin, p.y);
			ymax = std::max(ymax, p.y);
		}
	}
	double scale = std::max(xmax - xmin, ymax - ymin);
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

	for (auto v : boundary)
	{
		o << (((double) (v.x - xmin) * 560) / scale + 16) << " " << (((double)v.y * 560) / scale + 272) << " " << (first ? "m" : "l") << "\n";
		first = false;
	}
	o << "h\n</path>\n";

	for (auto poly : polygons)
	{
		o << "<path layer=\"candidates\" stroke=\"black\" fill=\"red\" opacity=\"25%\" stroke-opacity=\"opaque\">";

		bool first = true;

		for (auto v : poly)
		{
			o << (((double)(v.x - xmin) * 560) / scale + 16) << " " << (((double) v.y * 560) / scale + 272) << " " << (first ? "m" : "l") << "\n";
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

void pathsToIPE(std::string path, Path boundary, std::vector<Paths> polygons) {
	std::vector<Path> flattened;
	for (auto polys : polygons) {
		for (auto poly : polys) {
			flattened.push_back(poly);
		}
	}
	pathsToIPE(path, boundary, flattened);
}

void toIPE2(std::string path, Polygon boundary, std::vector<std::vector<Polygon>> polygons, std::vector<Paths> paths)
{
	std::vector<Path> flattened;
	for (auto polys : paths) {
		for (auto poly : polys) {
			flattened.push_back(poly);
		}
	}
	for (auto polys : polygons) {
		for (auto poly : polys) {
			Path newPath;
			for (auto p : poly)
			{
				newPath.push_back({ p.x().interval().sup(), p.y().interval().sup() });
			}
			flattened.push_back(newPath);
		}
	}
	Path boundaryPath;
	for (auto p : boundary)
	{
		boundaryPath.push_back({ p.x().interval().sup(), p.y().interval().sup() });
	}
	pathsToIPE(path, boundaryPath, flattened);
}

SolveStatus IncrementalNoFitSolver::solve(Problem* prob)
{
	problem = prob;
	int c = 0;
	for(size_t i = 0; i < problem->getNumItems(); i += batchSize) {
		initNoFits(i);
		additionalInits();
		while (true)
		{
			ItemWithNoFit* bestItem;
			Point64 translation;

			if (!findBestItem(bestItem, translation)) {
				break;
			}
			
			--bestItem->item->quantity;
			if (bestItem->item->quantity <= 0)
				itemsWithNoFit.remove(bestItem);

			addNewPiece(bestItem, translation);
			++c;
			std::cout << c << " pieces placed\n";
		}
		//toIPE2("test.ipe", problem->getContainer(), { placedPieces }, {});
		break;
	}
	return SolveStatus::Feasible;
}


void IncrementalNoFitSolver::initNoFits(size_t index) {
	//Initialize no-fit polygon for each item as polygon around container with a hole for each free space

	container = problem->getContainer();

	for (auto v : container) {
		pathContainer.push_back({ v.x().interval().sup(), v.y().interval().sup() });
	}

	int wall = 10e6;

	//build new polygon as outside of container

	auto bottom = container.bottom_vertex();
	auto top = container.top_vertex();

	container.reverse_orientation();
	auto containerStart = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.top_vertex());
	auto containerEnd = containerStart - 1;

	auto dist = CGAL::squared_distance(*containerStart, *containerEnd);
	containerEnd = CGAL::Circulator_from_iterator(container.begin(), container.end(), container.insert(containerStart.current_iterator(),
		{ containerStart->x() + (containerEnd->x() - containerStart->x()) / dist, containerStart->y() + (containerEnd->y() - containerStart->y()) / dist }));
	containerStart = containerEnd + 1;

	auto it = container.insert(containerStart.current_iterator(), { containerStart->x(), containerStart->y() + wall });
	it = container.insert(it, { container.right_vertex()->x() + wall, it->y() });
	it = container.insert(it, { it->x(), container.bottom_vertex()->y() - wall });
	it = container.insert(it, { container.left_vertex()->x() - wall, it->y() });
	it = container.insert(it, { it->x(), container.top_vertex()->y() });

	int c = 0;
	items = problem->getItems();
	size_t end = std::min(items.size(), index + batchSize);
	for (size_t i = index; i < end; i++) {
		Polygon inverse;
		Partition_traits::Polygon_2 polyIndices;
		for (size_t j = 0; j < items[i]->poly.vertices().size(); j++)
		{
			inverse.push_back({ -items[i]->poly[j].x(), -items[i]->poly[j].y() });
			polyIndices.push_back(j);
		}
		std::vector<Point> points = items[i]->poly.vertices();
		Partition_traits traits(CGAL::make_property_map(points));
		std::list<Partition_traits::Polygon_2> polyDecompIndices;
		CGAL::optimal_convex_partition_2(polyIndices.vertices_begin(), polyIndices.vertices_end(), std::back_inserter(polyDecompIndices), traits);

		//check if decomposition failed, for some reason that happens sometimes
		if (!CGAL::convex_partition_is_valid_2(polyIndices.vertices_begin(),
			polyIndices.vertices_end(),
			polyDecompIndices.begin(),
			polyDecompIndices.end(),
			traits)){
			std::cout << "optimal decomposition not found for polygon  " << items[i]->index << "\n";
			polyDecompIndices = {};
			CGAL::approx_convex_partition_2(polyIndices.vertices_begin(), polyIndices.vertices_end(), std::back_inserter(polyDecompIndices), traits);
			assert(CGAL::convex_partition_is_valid_2(polyIndices.vertices_begin(),
				polyIndices.vertices_end(),
				polyDecompIndices.begin(),
				polyDecompIndices.end(),
				traits));
		}

		std::vector<ConvexEdgeList> polyDecomp;
		std::vector<ConvexEdgeList> inverseDecomp;
		Paths decompPaths;
		Paths inverseDecompPaths;

		for (auto partition : polyDecompIndices) {
			auto previous = CGAL::Circulator_from_iterator(partition.begin(), partition.end(), partition.begin());
			auto start = previous;
			auto leftPoint = previous;
			//find leftmost point
			do {
				if (points[*previous] < points[*leftPoint])
					leftPoint = previous;
				++previous;
			} while (previous != start);

			previous = leftPoint;
			auto next = previous;
			++next;

			start = previous;
			bool rightHalf = true;
			Point64 startPoint(points[*previous].x().interval().sup(), points[*previous].y().interval().sup());
			std::vector<EdgeVector> rightEdges;
			std::vector<EdgeVector> leftEdges;

			Point64 inverseStartPoint;
			std::vector<EdgeVector> inverseRightEdges;
			std::vector<EdgeVector> inverseLeftEdges;
			do {
				int64_t x = (points[*next].x() - points[*previous].x()).interval().sup();
				int64_t y = (points[*next].y() - points[*previous].y()).interval().sup();
				double slope = x == 0 ? std::numeric_limits<double>::max() : ((double)y) / ((double)x);
				EdgeVector ev({ Point64(x, y), slope });
				EdgeVector evInverse({ Point64(-x, -y), slope });

				if (rightHalf && x < 0) {
					rightHalf = false;
					inverseStartPoint = Point64(-points[*previous].x().interval().sup(), -points[*previous].y().interval().sup());
				}
				if (rightHalf) {
					rightEdges.push_back(ev);
					inverseLeftEdges.push_back(evInverse);
				}
				else {
					leftEdges.push_back(ev);
					inverseRightEdges.push_back(evInverse);
				}
				++previous;
				++next;
			} while (previous != start);
			polyDecomp.push_back({ startPoint, rightEdges, leftEdges});
			inverseDecomp.push_back({ inverseStartPoint, inverseRightEdges, inverseLeftEdges });
		}

		//pathsToIPE("test.ipe", container, { inverseNext });
		auto innerFitHoles = CGAL::minkowski_sum_2(container, inverse).holes();
		Paths innerFit;

		for (auto innerFitHole: innerFitHoles) {
			innerFitHole.reverse_orientation();
			Path hole;
			for (auto p: innerFitHole)
			{
				hole.push_back({ p.x().interval().sup(), p.y().interval().sup() });
			}
			innerFit.push_back(hole);
		}
		
		itemsWithNoFit.push_back(new ItemWithNoFit({ items[i], innerFit, polyDecomp, inverseDecomp }));
		++c;
	}
}

void IncrementalNoFitSolver::updateNoFits(ItemWithNoFit* addedPiece, Point64& translation) {

	//update no-fit polygons
	
	long t1 = 0, t2 = 0;
	for (auto it : itemsWithNoFit)
	{
		auto start = std::chrono::high_resolution_clock::now();
		Paths noFitParts = Paths({});
		for (auto addedPart : addedPiece->convexDecomp)
		{
			for (auto inversePart : it->inversePoly)
			{
				Point64 previous = addedPart.start + inversePart.start + translation;
				Path currentNoFit({ previous });

				size_t i = 0, j = 0;
				while (i < addedPart.rightEdges.size() || j < inversePart.rightEdges.size())
				{
					if (j >= inversePart.rightEdges.size() || (i < addedPart.rightEdges.size() && addedPart.rightEdges[i].slope <= inversePart.rightEdges[j].slope)) {
						previous = addedPart.rightEdges[i].vec + previous;
						currentNoFit.push_back(previous);
						++i;
					}
					else {
						previous = inversePart.rightEdges[j].vec + previous;
						currentNoFit.push_back(previous);
						++j;
					}
				}
				i = 0, j = 0;
				while (i < addedPart.leftEdges.size() || j < inversePart.leftEdges.size())
				{
					if (j >= inversePart.leftEdges.size() || (i < addedPart.leftEdges.size() && addedPart.leftEdges[i].slope <= inversePart.leftEdges[j].slope)) {
						previous = addedPart.leftEdges[i].vec + previous;
						currentNoFit.push_back(previous);
						++i;
					}
					else {
						previous = inversePart.leftEdges[j].vec + previous;
						currentNoFit.push_back(previous);
						++j;
					}
				}
				noFitParts.push_back(currentNoFit);
			}
		}
		auto mid = std::chrono::high_resolution_clock::now();
		Paths noFit = Clipper2Lib::Union(noFitParts, Clipper2Lib::FillRule::NonZero);
		//pathsToIPE("test.ipe", container, { {noFitPath}, it->innerFit });
		it->innerFit = Clipper2Lib::Difference(it->innerFit, noFit, Clipper2Lib::FillRule::NonZero);
		//pathsToIPE("test.ipe", container, it->innerFit);
		auto stop = std::chrono::high_resolution_clock::now();
		t1 += std::chrono::duration_cast<std::chrono::nanoseconds>(mid - start).count();
		t2 += std::chrono::duration_cast<std::chrono::nanoseconds>(stop - mid).count();
	}
	std::cout << t1 / 1000000 << "  " << t2 / 1000000 << "\n";
}

void IncrementalNoFitSolver::addNewPiece(ItemWithNoFit* item, Point64& translation) {
	Transformation translate(CGAL::TRANSLATION, Vector(NT((long) translation.x), NT((long) translation.y)));
	Candidate addedCand({item->item->index, transform(translate, item->item->poly), (long) translation.x, (long) translation.y});
	problem->addCandidate(addedCand, item->item->value);
	updateNoFits(item, translation);
	additionalUpdates(item, translation);
	//placedPieces = Clipper2Lib::Union(placedPieces, { newPiece }, Clipper2Lib::FillRule::NonZero);
}


bool IncrementalNoFitSolver::findBestItem(ItemWithNoFit*& bestItem, Point64& translation) {
	bool itemFound = false;

	int64_t bestEval = evalIsBetter(1, 0) ? std::numeric_limits<int64_t>::min() : std::numeric_limits<int64_t>::max();
	for (auto it = itemsWithNoFit.begin(); it!=itemsWithNoFit.end();) {
		Clipper2Lib::Point64 attachmentPoint;
		if (findBestPlacement(*it, attachmentPoint)) {
			itemFound = true;
			Transformation translate(CGAL::TRANSLATION, Vector((long) attachmentPoint.x, (long) attachmentPoint.y));
			Polygon currentPlaced = transform(translate, (*it)->item->poly);

			int64_t currentEval = evalPlacement(currentPlaced, attachmentPoint, (*it)->item->value);
			if (evalIsBetter(currentEval, bestEval)) {
				bestEval = currentEval;
				bestItem = (*it);
				translation = attachmentPoint;
			}
			++it;
		}
		else {
			it = itemsWithNoFit.erase(it);
		}
	}
	return itemFound;
}

bool IncrementalNoFitSolver::findBestPlacement(ItemWithNoFit* testedItem, Point64& attachmentPoint) {
	//find best position for given item inside current container

	bool positionFound = false;
	attachmentPoint = Point64(std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::min());
	for (auto area : testedItem->innerFit) {
		for (auto p : area) {
			if (p.x < attachmentPoint.x || (p.x == attachmentPoint.x && p.y < attachmentPoint.y)) {
				attachmentPoint = p;
				positionFound = true;
			}
		}
	}

	return positionFound;
}

int64_t IncrementalNoFitSolver::evalPlacement(Polygon& placedPoly, Point64& translation, int value) {
	int64_t eval = 0;
	
	eval += translation.x * translation.x + translation.y * translation.y;// -value;

	return eval;
}


/*void IncrementalNoFitSolver::getHullVacancies(const Polygon& poly, std::vector<Polygon>& vacancies) {

	auto vertices = poly.vertices();
	//calculate convex hull
	std::vector<std::size_t> indices(poly.size()), CHIndices;
	std::iota(indices.begin(), indices.end(), 0);
	CGAL::convex_hull_2(indices.begin(), indices.end(), std::back_inserter(CHIndices),
		Convex_hull_traits(CGAL::make_property_map(vertices)));

	size_t start = CHIndices[0] + 1;
	if (start >= vertices.size())
		start = 0;
	size_t i = start;
	size_t hullPrev = 0;
	size_t hullNext = 1;
	Polygon vacancy;
	bool insideConcave = false;

	//iterate polygon to find parts not contained in convex hull
	do {
		if (i != CHIndices[hullNext] && !CGAL::collinear(vertices[i], vertices[CHIndices[hullPrev]], vertices[CHIndices[hullNext]])) {
			//Concave part
			if (!insideConcave) {
				insideConcave = true;
				if (i == 0)
					vacancy.push_back(vertices[vertices.size() - 1]);
				else
					vacancy.push_back(vertices[i - 1]);
			}
			if (insideConcave)
				vacancy.push_back(vertices[i]);
		}
		else {
			//point on convex hull
			if (insideConcave) {
				insideConcave = false;
				vacancy.push_back(vertices[i]);
				vacancy.reverse_orientation();
				vacancies.push_back(vacancy);
				vacancy = Polygon();
			}
			if (i == CHIndices[hullNext]) {
				hullPrev = hullNext;
				++hullNext;
				if (hullNext >= CHIndices.size())
					hullNext = 0;
			}
		}
		++i;
		if (i >= vertices.size())
			i = 0;
	} while (i != start);
}*/