#include "problem.hpp"

// // bool Problem::isValidPacking()
// // {

// //     for (int i = 0; i < packing_polygons.size(); i++)
// //     {
// //         Polygon packer = packing_polygons.at(i);

// //         // Test that all polygons are inside the boundary
// //         if (CGAL::do_intersect(packer, boundary_polgyon))
// //         {
// //             return false;
// //         }

// //         std::cout << *(packer.begin()) << std::endl;

// //         if (boundary_polgyon.has_on_bounded_side(*(packer.begin())))
// //         {
// //             return false;
// //         }

// //         // Test that all packing polygons do not intersect
// //         for (int j = i + 1; j < packing_polygons.size(); j++)
// //         {
// //             Polygon packer2 = packing_polygons.at(j);

// //             if (CGAL::do_intersect(packer, packer2))
// //             {
// //                 return false;
// //             }

// //             if (packer2.has_on_unbounded_side(*(packer.begin())))
// //             {
// //                 return false;
// //             }
// //         }
// //     }

// //     return true;
// // }

Problem::Problem(char *file_name)
{
    std::ifstream f(file_name);
    json data = json::parse(f);

    type = data["type"];
    name = data["instance_name"];
    num_items = data["num_items"];

    // n_items = data["num_items"];

    auto cont_x = data["container"]["x"];
    auto cont_y = data["container"]["y"];

    for (int i = 0; i < cont_x.size(); i++)
    {
        Point t((int)cont_x[i], (int)cont_y[i]);
        container.push_back(t);
    }

    for (auto item : data["items"])
    {
        auto x = item["x"];
        auto y = item["y"];

        Polygon it;

        for (int i = 0; i < x.size(); i++)
        {
            Point t((int)x[i], (int)y[i]);
            it.push_back(t);
        }

        items.push_back(Item(it, item["quantity"], item["value"]));
    }
}

void toIPE(std::string path, Polygon boundary, std::vector<Polygon> polygons)
{
    std::ofstream o(path);

    // Find extreme coords of the graph (if you use CGAL you can use inbuild functions instead)

    double xmin, xmax, ymin, ymax;
    for (Point p : boundary.vertices())
    {
        xmin = std::min(xmin, p.approx().x().pair().first);
        xmax = std::max(xmax, p.approx().x().pair().first);
        ymin = std::min(ymin, p.approx().y().pair().first);
        ymax = std::max(ymax, p.approx().y().pair().first);
    }
    float scale = std::max(xmax - xmin, ymax - ymin);

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

    // for (Point p : points)
    // {
    //     o << "<use layer=\"points\" name=\"mark/disk(sx)\" pos=\"" << ((p.x - xmin) * 560 / scale + 16) << " " << ((p.y - ymin) * 560 / scale + 272) << "\" size=\"normal\" stroke=\"black\"/>";
    //     // o << "<path layer=\"polygon\" stroke=\"black\">\n";
    //     // o << ((e.s.x - xmin) * 560 / scale + 16) << " " << ((e.s.y - ymin) * 560 / scale + 272) << " m\n";
    //     // o << ((e.t.x - xmin) * 560 / scale + 16) << " " << ((e.t.y - ymin) * 560 / scale + 272) << " l\n";
    //     // o << "h\n</path>\n";
    // }

    // for (int i = 1; i < boundary.size(); i++vis_name)
    // {
    //     o << ((boundary[i].x- xmin) * 560 / scale + 16) << " " << ((boundary[i].y - ymin) * 560 / scale + 272) << " l\n";
    // }

    // for (auto hole : holes)
    // {
    //     o << "<path layer=\"polygon\" stroke=\"black\" fill=\"gray\">\n";
    //     o << ((hole[0].x - xmin) * 560 / scale + 16) << " " << ((hole[0].y - ymin) * 560 / scale + 272) << " m\n";
    //     for (int i = 1; i < hole.size(); i++)
    //     {
    //         o << ((hole[i].x - xmin) * 560 / scale + 16) << " " << ((hole[i].y - ymin) * 560 / scale + 272) << " l\n";
    //     }
    //     o << "h\n</path>\n";
    // }

    // for (auto convex : cover)
    // {
    //     o << "<path layer=\"cover\" stroke=\"black\" fill=\"red\" opacity=\"25%\">\n";
    //     o << ((convex[0].x - xmin) * 560 / scale + 16) << " " << ((convex[0].y - ymin) * 560 / scale + 272) << " m\n";
    //     for (int i = 1; i < convex.size(); i++)
    //     {
    //         o << ((convex[i].x - xmin) * 560 / scale + 16) << " " << ((convex[i].y - ymin) * 560 / scale + 272) << " l\n";
    //     }
    //     o << "h\n</path>\n";
    // }

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

// void Problem::visualize()
// {
//     std::vector<Edge> edges;
//     std::vector<Point> points;
//     int gap = 10;

//     long long int start_x = 0;
//     int start_y = 0;

//     for (auto edge : container.edges())
//     {
//         Point s = edge.start();
//         Point t = edge.target();

//         edges.push_back(Edge(Point(s.x(), s.y()), Point(t.x(), t.y())));
//         points.push_back(Point(s.x(), s.y()));
//         points.push_back(Point(t.x(), t.y()));
//     }
//     start_y -= container.bbox().y_span() * 2;

//     for (Item item : items)
//     {
//         for (int i = 0; i < item.quantity; i++)
//         {
//             for (auto edge : item.poly.edges())
//             {
//                 auto s = edge.start();
//                 auto t = edge.target();

//                 double sx = s.approx().x().pair().first;
//                 double sy = s.approx().y().pair().first;
//                 double tx = t.approx().x().pair().first;
//                 double ty = t.approx().y().pair().first;

//                 edges.push_back(Edge(Point(sx + start_x, sy + start_y), Point(tx + start_x, ty + start_y)));
//                 points.push_back(Point(sx + start_x, sy + start_y));
//                 points.push_back(Point(tx + start_x, ty + start_y));
//             }

//             start_x += item.poly.bbox().x_span() + gap;
//         }
//     }

//     std::string path = name + ".ipe";

//     toIPE(path, edges, points);
//     // vis_name += name;
//     // vis_name += ".ipe";
// }

void Problem::visualizeSolution()
{
    std::vector<Edge> edges;
    std::vector<Point> points;

    for (auto edge : container.edges())
    {
        Point s = edge.start();
        Point t = edge.target();

        double sx = s.approx().x().pair().first;
        double sy = s.approx().y().pair().first;
        double tx = t.approx().x().pair().first;
        double ty = t.approx().y().pair().first;

        edges.push_back(Edge(Point(sx, sy), Point(tx, ty)));
        points.push_back(Point(sx, sy));
        points.push_back(Point(tx, ty));
    }

    for (Polygon candidate : candidates)
    {
        for (auto edge : candidate.edges())
        {
            Point s = edge.start();
            Point t = edge.target();

            double sx = s.approx().x().pair().first;
            double sy = s.approx().y().pair().first;
            double tx = t.approx().x().pair().first;
            double ty = t.approx().y().pair().first;

            edges.push_back(Edge(Point(sx, sy), Point(tx, ty)));
            points.push_back(Point(sx, sy));
            points.push_back(Point(tx, ty));
        }
    }

    std::string path = name + ".ipe";

    toIPE(path, container, candidates);
}