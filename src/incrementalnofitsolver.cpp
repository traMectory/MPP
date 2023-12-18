#include "incrementalnofitsolver.h"

void pathsToIPE(std::string path, Path boundary, std::vector<Path> polygons)
{
	std::ofstream o(path);

	// Find extreme coords of the graph (if you use CGAL you can use inbuild functions instead)

	NT xmin=999999999, xmax=-999999999, ymin = 9999999999, ymax=-999999999;
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
			Clipper2Lib::Path64 placedPoly;
			Clipper2Lib::Point64 translation;

			if (!findBestItem(bestItem, placedPoly, translation)) {
				break;
			}
			
			--bestItem->item->quantity;
			if (bestItem->item->quantity <= 0)
				itemsWithNoFit.remove(bestItem);

			//pathsToIPE("test.ipe", problem->getContainer(), { placedPieces,{ placedPoly }, bestItem->innerFit });
			addNewPiece(bestItem->item, placedPoly, translation);
			++c;
			//if (c % 100 == 0)
				//pathsToIPE("test.ipe", problem->getContainer(), placedPieces);
			std::cout << c << " pieces placed\n";
		}
		//toIPE2("test.ipe", problem->getContainer(), { placedPieces }, {});
		break;
	}
	return SolveStatus::Feasible;
}

/*void IncrementalNoFitSolver::precomputeNoFits(Item* item, Path& inversePoly, std::map<int, Paths>& result)
{
	for (auto other : items) {
		Paths noFit = Clipper2Lib::MinkowskiSum(other->poly, inversePoly, true);
		for (auto region : noFit) {
			if (Clipper2Lib::Area(region) > Clipper2Lib::Area(other->poly)) {
				noFit = { region };
				break;
			}
		}
		result.insert(std::pair<int, Paths>(other->index, noFit));
	}
	std::cout << item->index << ": noFits done\n";
}*/

void IncrementalNoFitSolver::initNoFits(size_t index) {
	//Initialize no-fit polygon for each item as polygon around container with a hole for each free space

	container = problem->getContainer();
	int wall = 10e6;
	int c = 0;
	items = problem->getItems();
	size_t end = std::min(items.size(), index + batchSize);
	for (size_t i = index; i < end; i++) {
		Path inverseNext;
		Polygon inverseCGAL;
		for (int j = 0; j < items[i]->poly.size(); j++)
		{
			Point p = items[i]->poly[j];
			inverseNext.push_back({ -p.x, -p.y });
			inverseCGAL.push_back({ (long)-p.x, (long)-p.y });
		}

		auto innerFit = Clipper2Lib::MinkowskiSum(container, inverseNext, true);
		for (auto region : innerFit) {
			if (Clipper2Lib::Area(region) < Clipper2Lib::Area(container)) {
				innerFit = { region };
				break;
			}
		}
		
		itemsWithNoFit.push_back(new ItemWithNoFit({ items[i], innerFit, inverseCGAL }));
		++c;
	}
}

void IncrementalNoFitSolver::updateNoFits(Path& addedPiece) {

	//update no-fit polygons
	Polygon addedPoly;
	for (auto p : addedPiece) {
		addedPoly.push_back({ (long)p.x, (long)p.y });
	}
	long t1 = 0, t2 = 0;
	for (auto it : itemsWithNoFit) {
		auto start = std::chrono::high_resolution_clock::now();
		Polygon noFitPoly = CGAL::minkowski_sum_2(addedPoly, it->inversePoly).outer_boundary();
		auto mid = std::chrono::high_resolution_clock::now();
		Path noFitPath;
		for (auto p : noFitPoly) {
			noFitPath.push_back( { p.x().interval().sup(), p.y().interval().sup() } );
		}
		//pathsToIPE("test.ipe", container, { {noFitPath}, it->innerFit });
		it->innerFit = Clipper2Lib::Difference(it->innerFit, { noFitPath }, Clipper2Lib::FillRule::NonZero);
		//pathsToIPE("test.ipe", container, it->innerFit);
		auto stop = std::chrono::high_resolution_clock::now();
		t1 += std::chrono::duration_cast<std::chrono::nanoseconds>(mid - start).count();
		t2 += std::chrono::duration_cast<std::chrono::nanoseconds>(stop - mid).count();
	}
	std::cout << t1 / 1000000 << "  " << t2 / 1000000 << "\n";
}

void IncrementalNoFitSolver::addNewPiece(Item* item, Path& newPiece, Point& translation) {
	Candidate addedCand({ item->index, newPiece, translation.x, translation.y });
	problem->addCandidate(addedCand, item->value);
	updateNoFits(newPiece);
	additionalUpdates(newPiece);
	placedPieces = Clipper2Lib::Union(placedPieces, { newPiece }, Clipper2Lib::FillRule::NonZero);
}


bool IncrementalNoFitSolver::findBestItem(ItemWithNoFit* &bestItem, Path& placedPoly, Point& translation) {
	bool itemFound = false;

	int64_t bestEval = evalIsBetter(1, 0) ? std::numeric_limits<int64_t>::min() : std::numeric_limits<int64_t>::max();
	for (auto it = itemsWithNoFit.begin(); it!=itemsWithNoFit.end();) {
		Clipper2Lib::Point64 attachmentPoint;
		if (findBestPlacement(*it, attachmentPoint)) {
			itemFound = true;
			Path currentPlaced = Clipper2Lib::TranslatePath((*it)->item->poly, attachmentPoint.x, attachmentPoint.y);

			int64_t currentEval = evalPlacement(currentPlaced, attachmentPoint, (*it)->item->value);
			if (evalIsBetter(currentEval, bestEval)) {
				bestEval = currentEval;
				bestItem = (*it);
				placedPoly = currentPlaced;
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

bool IncrementalNoFitSolver::findBestPlacement(ItemWithNoFit* testedItem, Clipper2Lib::Point64& attachmentPoint) {
	//find best position for given item inside current container

	bool positionFound = false;
	attachmentPoint = Clipper2Lib::Point64(std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::min());
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

int64_t IncrementalNoFitSolver::evalPlacement(Clipper2Lib::Path64& placedPoly, Clipper2Lib::Point64& translation, int value) {
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