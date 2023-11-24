#pragma once

#include "bottom_left.h"

SolveStatus BottomLeft::solve(Problem *prob) {
    return BottomLeft::bl(prob);
}

SolveStatus BottomLeft::bl(Problem *prob) {
    Polygon container = prob->getContainer();
    Polygon_with_holes complement;
    CGAL::complement(container, complement);
    Item *item;
    for (int i = 0; i < prob->getNumItems(); i++) {
        item = prob->getItems()[i];
        while (item->quantity > 0) {
            complement = pack_into(item, complement, prob, i);
            item->quantity--;
        }
    }

    return SolveStatus::Feasible;
}

Polygon_with_holes BottomLeft::pack_into(Item *item, Polygon_with_holes c, Problem *prob, int i) {
    Polygon inv = inverse(item->poly);
    Polygon_with_holes inner_fit_space = CGAL::minkowski_sum_2(inv, c);
    if (!inner_fit_space.has_holes()) {
        return c;
    }
    Polygon hole = inner_fit_space.holes()[0];
    Point bottom_left = hole.bottom_vertex()[0];
    Polygon translated = translate_by(item->poly, bottom_left);

    Polygon_with_holes packed_inner_fit_space;
    CGAL::join(translated, c, packed_inner_fit_space, CGAL::Tag_false());

    Candidate cand{
        .index = i,
        .poly = translated,
        .x_translation = 
            (double) (bottom_left.x().interval().pair().first),
        .y_translation =
            (double) (bottom_left.y().interval().pair().first),
    };
    prob->addCandidate(cand, item->value);

    return packed_inner_fit_space;
}

Polygon BottomLeft::translate_by(Polygon& p, Point& t) {
    Polygon translated;
    Point point;
    for (int i = 0; i < p.size(); i++) {
        point = p.vertices()[i];
        auto newP = Point(point.x() + t.x(), point.y() + t.y());
        translated.push_back(newP);
    }
    return translated;
}

Polygon BottomLeft::inverse(Polygon& p) {
    Polygon inverse;
    for (int i = 0; i < p.size(); i++) { 
        Point point = Point(-p.vertices()[i].x(), -p.vertices()[i].y());
        inverse.push_back(point);
    }
    return inverse;
}