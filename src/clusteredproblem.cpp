#include "clusteredproblem.h"
#include "toposv2.h"

ClusteredProblem::ClusteredProblem(char* file_name) : Problem(file_name)
{
	/*originalItems = items;
	for (size_t i = 0; i < originalItems.size(); i++)
	{
		//create cluster for each item containing only itself
		items[i] = new Cluster( -1, originalItems[i]->poly, {}, originalItems[i]->quantity, originalItems[i]->value , { originalItems[i] }, { 0 }, { 0 });
	}*/
	cluster();
}

void ClusteredProblem::addCandidate(Item* item, NT x_translation, NT y_translation) {
	if (item->index == -1) {
		//candidate is cluster
		Cluster* c = (Cluster*)item;
		for (size_t i = 0; i < c->packedItems.size(); i++)
		{
			Problem::addCandidate(c->packedItems[i], c->x_translations[i] + x_translation, c->y_translations[i] + y_translation);
		}
	}
	else {
		//candidate is normal item
		Problem::addCandidate(item, x_translation, y_translation);
	}
}

void ClusteredProblem::cluster()
{
	for (size_t l = 0; l < 1; l++)
	{
		int length = items.size();
		for (size_t i = 0; i < length; i++)
		{
			if (items[i]->quantity > 0 && !items[i]->poly.is_convex()) {
				NT bestEval;
				Point bestAttachment;
				size_t bestIndex;
				Polygon bestPoly;

				std::vector<Polygon> vacancies1;
				getHullVacancies(items[i]->poly, vacancies1);
				//toIPE2("test.ipe", items[i]->poly, vacancies1, {});

				for (size_t j = 0; j < items.size(); j++)
				{
					if (items[j]->quantity > 0 && (i != j || items[i]->quantity > 1)) {
						Point attachmentPoint;
						attachPoly(items[i], items[j], attachmentPoint);

						//HACK: placedPoly causes errors in debug mode, currently only works in release
						Transformation translate(CGAL::TRANSLATION, Vector(attachmentPoint.x(), attachmentPoint.y()));
						Polygon placedPoly = transform(translate, items[j]->poly);

						//evaluate cluster according to two criteria
						NT eval = 0;

						std::vector<Polygon> vacancies2;
						getHullVacancies(placedPoly, vacancies2);
						//if (j == 876)
						//	toIPE2("test.ipe", placedPoly, vacancies2, {});

						//criterion 1 - intersection of convex hull vacancies
						NT c1 = 0;
						for (auto v : vacancies1)
						{
							std::vector<Polygon_with_holes> occupied;
							CGAL::intersection(placedPoly, v, std::back_inserter(occupied));
							if (occupied.size() > 0) {
								NT area = 0;
								for (auto occ : occupied) {
									area += occ.outer_boundary().area();
								}
								c1 += area / v.area();
							}
						}
						NT c2 = 0;

						for (auto v : vacancies2)
						{
							//toIPE2("test.ipe", hull2, { items[i]->poly, v }, {});
							std::vector<Polygon_with_holes> occupied;
							CGAL::intersection(items[i]->poly, v, std::back_inserter(occupied));
							if (occupied.size() > 0) {
								NT area = 0;
								for (auto occ : occupied) {
									area += occ.outer_boundary().area();
								}
								c2 += area / v.area();
							}
						}

						eval += std::max(c1, c2);

						//criterion 2 - utilization of joined convex hull
						Polygon joinedHull;
						std::vector<Point> points;
						for (auto vert : items[i]->poly.vertices()) {
							points.push_back(vert);
						}
						for (auto vert : placedPoly.vertices()) {
							points.push_back(vert);
						}
						CGAL::convex_hull_2(points.begin(), points.end(), std::back_inserter(joinedHull));
						eval += (items[i]->poly.area() + placedPoly.area()) / joinedHull.area();

						if (eval > bestEval) {
							bestEval = eval;
							bestIndex = j;
							Polygon_with_holes joinedPoly;
							CGAL::join(items[i]->poly, placedPoly, joinedPoly);
							bestPoly = joinedPoly.outer_boundary();
							bestAttachment = attachmentPoint;
						}
					}
				}
				if (bestEval > 2) {
					//cluster has been found
					//toIPE2("test.ipe", items[i]->poly, { bestPoly }, {});
					size_t quantity = i != bestIndex ? std::min(items[i]->quantity, items[bestIndex]->quantity) : items[i]->quantity / 2;
					items[i]->quantity -= quantity;
					items[bestIndex]->quantity -= quantity;

					//create new clustered item
					std::vector<Item*> contained;
					std::vector<NT> x_translations;
					std::vector<NT> y_translations;

					//add fixed item to new cluster
					if (items[i]->index == -1) {
						//contained item is a cluster itself
						Cluster* c = (Cluster*)items[i];
						for (size_t k = 0; k < c->packedItems.size(); k++)
						{
							contained.push_back(c->packedItems[k]);
							x_translations.push_back(c->x_translations[k]);
							y_translations.push_back(c->y_translations[k]);
						}
					}
					else {
						//contained item is not a cluster
						contained.push_back(items[i]);
						x_translations.push_back(0);
						y_translations.push_back(0);
					}

					//add movable item to new cluster
					if (items[bestIndex]->index == -1) {
						//contained item is a cluster itself
						Cluster* c = (Cluster*)items[bestIndex];
						for (size_t k = 0; k < c->packedItems.size(); k++)
						{
							contained.push_back(c->packedItems[k]);
							x_translations.push_back(c->x_translations[k] + bestAttachment.x());
							y_translations.push_back(c->y_translations[k] + bestAttachment.y());
						}
					}
					else {
						//contained item is not a cluster
						contained.push_back(items[bestIndex]);
						x_translations.push_back(bestAttachment.x());
						y_translations.push_back(bestAttachment.y());
					}

					Cluster* newCluster = new Cluster(-1, bestPoly, {}, quantity, items[i]->value + items[bestIndex]->value, contained, x_translations, y_translations);

					//delete old items if quantity is 0 now
					//TODO: inefficient deletion using vector for items, maybe switch to list instead
					if (i != bestIndex) {
						if (items[bestIndex]->quantity == 0) {
							items.erase(items.begin() + bestIndex);
							--length;
							if (bestIndex < i)
								--i;
						}
						if (items[i]->quantity == 0) {
							items.erase(items.begin() + i);
							--i;
							--length;
						}
					}
					else if (items[i]->quantity == 0) {
						items.erase(items.begin() + i);
						--i;
						--length;
					}

					items.push_back(newCluster);
					std::cout << items.size() << std::endl;
				}
			}
		}
	}
	std::cout << "done\n";
}

bool ClusteredProblem::attachPoly(Item* fixed, Item* movable, Point& dominantPoint)
{
	//

	//Calculate No-Fit Polygon for movable polygon around fixed
	Polygon inverseMovable;
	for (int i = 0; i < movable->poly.size(); i++)
	{
		Point p = movable->poly.vertices()[i];
		inverseMovable.push_back(Point(-p.x(), -p.y()));
	}
	//Points on boundary of noFit Polygon
	auto noFit = CGAL::minkowski_sum_2(fixed->poly, inverseMovable).outer_boundary().vertices();

	//Find Points of NFP that are part of convex hull
	std::vector<std::size_t> indices(noFit.size()), CHIndices;
	std::iota(indices.begin(), indices.end(), 0);
	CGAL::convex_hull_2(indices.begin(), indices.end(), std::back_inserter(CHIndices),
		Convex_hull_traits(CGAL::make_property_map(noFit)));

	//Find Point on NFP boundary farthest from opposing CH edge
	NT maxDist = 0;
	size_t start = CHIndices[0];
	size_t i = start + 1;
	size_t hullPrev = 0;
	size_t hullNext = 1;
	while (i != start) {
		if (i != CHIndices[hullNext]) {
			//Concave part
			NT dist = CGAL::squared_distance(noFit[i], Line(noFit[CHIndices[hullPrev]], noFit[CHIndices[hullNext]]));
			if (dist > maxDist) {
				maxDist = dist;
				dominantPoint = noFit[i];
			}
		}
		else {
			//point on convex hull
			hullPrev = hullNext;
			++hullNext;
			if (hullNext >= CHIndices.size())
				hullNext = 0;
		}
		++i;
		if (i >= noFit.size())
			i = 0;
	}
	return true;
}


void ClusteredProblem::getHullVacancies(const Polygon& poly, std::vector<Polygon>& vacancies) {
	
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
					vacancy.push_back(vertices[i-1]);
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