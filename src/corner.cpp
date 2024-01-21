#include "corner.h"

Corner::Corner(std::vector<Strategy> strategies) {
    this->strategies = strategies;
}

SolveStatus Corner::solve(Problem *prob) {
    this->prob = prob;
    SolveStatus status = corner();
    return status;
}

SolveStatus Corner::corner() {
    Polygon_with_holes complement;
    CGAL::complement(prob->getContainer(), complement);
    std::vector<Item> items_copy;
    for (int i = 0; i < prob->getItems().size(); i++) {
        items_copy.push_back(*prob->getItems()[i]);
    }
    Item item;
    for (int i = 0; i < items_copy.size(); i++) {
        item = items_copy[i];
        while (item.quantity > 0) {
            complement = pack_into(&item, complement, strategies[i]);
            item.quantity--;
        }
    }

    return SolveStatus::Feasible;
}

Polygon_with_holes Corner::pack_into(Item *item, Polygon_with_holes c, Strategy strategy) {
    Polygon inv = inverse(item->poly);
    Polygon_with_holes inner_fit_space = CGAL::minkowski_sum_2(inv, c);
    if (!inner_fit_space.has_holes()) {
        return c;
    }
    Polygon hole = inner_fit_space.holes()[0];
    Point corner;
    switch (strategy)
    {
    case BL:
        corner = hole.bottom_vertex()[0];
        break;
    case TL:
        corner = hole.left_vertex()[0];
        break;
    case TR:
        corner = hole.top_vertex()[0];
        break;
    case BR:
        corner = hole.right_vertex()[0];
        break;
    default:
        break;
    }
    Polygon translated = translate_by(item->poly, corner);

    Polygon_with_holes packed_inner_fit_space;
    CGAL::join(translated, c, packed_inner_fit_space, CGAL::Tag_false());

    Candidate cand{
        .index = item->index,
        .poly = translated,
        .x_translation = 
            (double) (corner.x().interval().pair().first),
        .y_translation =
            (double) (corner.y().interval().pair().first),
    };
    prob->addCandidate(cand, item->value);

    return packed_inner_fit_space;
}

Polygon Corner::translate_by(Polygon& p, Point& t) {
    Polygon translated;
    Point point;
    for (int i = 0; i < p.size(); i++) {
        point = p.vertices()[i];
        auto newP = Point(point.x() + t.x(), point.y() + t.y());
        translated.push_back(newP);
    }
    return translated;
}

Polygon Corner::inverse(Polygon& p) {
    Polygon inverse;
    for (int i = 0; i < p.size(); i++) { 
        Point point = Point(-p.vertices()[i].x(), -p.vertices()[i].y());
        inverse.push_back(point);
    }
    return inverse;
}