#include "incrementalnofitsolver.h"
#include "toposv2.h"

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
			Polygon placedPoly;
			Point translation;

			if (!findBestItem(bestItem, placedPoly, translation)) {
				break;
			}

			--bestItem->item->quantity;
			if (bestItem->item->quantity <= 0)
				itemsWithNoFit.remove(bestItem);

			addNewPiece(bestItem->item, placedPoly, translation);

			++c;
			std::cout << c << " pieces placed\n";
		}
		toIPE2("test.ipe", problem->getContainer(), { placedPieces }, {});
		break;
	}
	return SolveStatus::Feasible;
}

void IncrementalNoFitSolver::initNoFits(size_t index) {
	//Initialize no-fit polygon for each item as polygon around container with a hole for each free space

	Polygon container = problem->getContainer();
	int wall = 10e6;
	Point corners[] = { Point(container.left_vertex()->x() - wall, container.top_vertex()->y() + wall),
		Point(container.left_vertex()->x() - wall, container.bottom_vertex()->y() - wall),
		Point(container.right_vertex()->x() + wall, container.bottom_vertex()->y() - wall),
		Point(container.right_vertex()->x() + wall, container.top_vertex()->y() + wall) };
	Polygon box(corners, corners + 4);
	Polygon_with_holes inverseContainer(box);
	container.reverse_orientation();
	inverseContainer.add_hole(container);
	int c = 0;
	auto items = problem->getItems();
	size_t end = std::min(items.size(), index + batchSize);
	for (size_t i = index; i < end; i++) {
		Polygon inverseNext;
		for (int j = 0; j < items[i]->poly.size(); j++)
		{
			Point p = items[i]->poly.vertices()[j];
			inverseNext.push_back(Point(-p.x(), -p.y()));
		}

		Polygon_with_holes noFitContainer = CGAL::minkowski_sum_2(inverseContainer, inverseNext);

		Polygon_with_holes noFit;
		if (index > 0) {
			CGAL::join(noFitContainer, CGAL::minkowski_sum_2(placedPieces, inverseNext), noFit);
		}
		else
			noFit = noFitContainer;
		itemsWithNoFit.push_back(new ItemWithNoFit({ items[i], noFit, inverseNext }));
		++c;
	}
}

void IncrementalNoFitSolver::updateNoFits(Candidate& addedCand) {

	long long timer = 0;
	int complexity = 0;
	//update no-fit polygons
	for (auto it : itemsWithNoFit) {
		Polygon noFitNew = CGAL::minkowski_sum_2(addedCand.poly, it->inversePoly).outer_boundary();
		Polygon_with_holes joinedNoFit;
		auto start = std::chrono::system_clock::now();
		CGAL::join(it->noFit, noFitNew, joinedNoFit);
		auto end = std::chrono::system_clock::now();
		timer += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		it->noFit = joinedNoFit;
		complexity += it->noFit.holes()[0].size();
	}
	std::cout << complexity << ": " << timer / 1000000 << "\n";
}

void IncrementalNoFitSolver::addNewPiece(Item* item, Polygon& newPiece, Point& translation) {
	Candidate addedCand({ item->index, newPiece, translation.x(), translation.y() });
	problem->addCandidate(addedCand, item->value);
	updateNoFits(addedCand);
	additionalUpdates(addedCand);
	Polygon_with_holes newPlaced;
	if (CGAL::join(placedPieces, newPiece, newPlaced))
		placedPieces = newPlaced.outer_boundary();
	else
		placedPieces = newPiece;
}


bool IncrementalNoFitSolver::findBestItem(ItemWithNoFit* &bestItem, Polygon& placedPoly, Point& translation) {
	bool itemFound = false;

	NT bestEval = evalIsBetter(1, 0) ? 0 : 10e40;
	for (auto it : itemsWithNoFit) {
		Point attachmentPoint;
		if (findBestPlacement(it, attachmentPoint)) {
			itemFound = true;
			Transformation translate(CGAL::TRANSLATION, Vector(attachmentPoint.x(), attachmentPoint.y()));
			Polygon currentPlaced = transform(translate, it->item->poly);

			NT currentEval = evalPlacement(currentPlaced, it->item->value);
			if (evalIsBetter(currentEval, bestEval)) {
				bestEval = currentEval;
				bestItem = it;
				placedPoly = currentPlaced;
				translation = attachmentPoint;
			}
		}
		else {
			itemsWithNoFit.remove(it);
		}
	}
	return itemFound;
}

bool IncrementalNoFitSolver::findBestPlacement(ItemWithNoFit* testedItem, Point& attachmentPoint) {
	//find best position for given item inside current container

	bool positionFound = false;
	attachmentPoint = Point(testedItem->noFit.outer_boundary().right_vertex()->x(), testedItem->noFit.outer_boundary().top_vertex()->y());
	for (auto hole : testedItem->noFit.holes()) {
		for (auto p : hole) {
			if (p < attachmentPoint) {
				attachmentPoint = p;
				positionFound = true;
			}
		}
	}

	return positionFound;
}

NT IncrementalNoFitSolver::evalPlacement(Polygon& placedPoly, int value) {
	NT eval = 0;
	
	CGAL::squared_distance(Point(placedPoly.right_vertex()->x(), placedPoly.top_vertex()->y()), bottomLeft) - value;

	return eval;
}


void IncrementalNoFitSolver::getHullVacancies(const Polygon& poly, std::vector<Polygon>& vacancies) {

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
}