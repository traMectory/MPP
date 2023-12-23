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

    if (f.fail())
    {
        std::cerr << "Fail not found!\n";
        return;
    }

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

    int index = 0;

    // items = new std::vector<Item>();

    for (auto item : data["items"])
    {
        auto x = item["x"];
        auto y = item["y"];

        Path it;

        for (int i = 0; i < x.size(); i++)
        {
            Point t((int)x[i], (int)y[i]);
            it.push_back(t);
        }

        items.push_back(new Item({ index, it, {}, item["quantity"], item["value"] }));
        index++;
    }
}

void Problem::storeSolution(std::string loc)
{
    std::string vis_name = loc + "/" + name + ".cgshop2024_solution.json";
    std::ofstream o(vis_name);

    json output;

    output["type"] = "cgshop2024_solution";

    output["instance_name"] = name;
    output["item_indices"] = {};
    output["x_translations"] = {};
    output["y_translations"] = {};
    output["comments"] = {};

    for (std::string comment : comments)
    {
        output["comments"].push_back(comment);
    }

    for (Candidate item : candidates)
    {
        output["item_indices"].push_back(item.index);
        output["x_translations"].push_back(item.x_translation);
        output["y_translations"].push_back(item.y_translation);
        // output["x_translations"].push_back(item.x_translation.exact().numerator().doubleValue()/item.x_translation.exact().denominator().doubleValue());
        // output["y_translations"].push_back(item.y_translation.exact().numerator().doubleValue()/item.y_translation.exact().denominator().doubleValue());
    }

    o << std::setw(4) << output << std::endl;
    o.close();
}

void toIPE(std::string path, Path boundary, std::vector<Candidate> polygons)
{
    std::ofstream o(path);

    // Find extreme coords of the graph (if you use CGAL you can use inbuild functions instead)

    double xmin = std::numeric_limits<double>::max(),
        xmax = std::numeric_limits<double>::min(),
        ymin = std::numeric_limits<NT>::max(),
        ymax = std::numeric_limits<double>::min();
    for (Point p : boundary)
    {

        xmin = std::min(xmin, (double) p.x);
        xmax = std::max(xmax, (double) p.x);
        ymin = std::min(ymin, (double) p.y);
        ymax = std::max(ymax, (double) p.y);
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
    o << "<view layers=\"boundary candidates\" active=\"candidates\"/>\n";

    o << "<path layer=\"boundary\" stroke=\"black\">";

    bool first = true;

    for (Point v : boundary)
    {
        o << ((v.x - xmin) * 560 / scale + 16) << " " << (v.y * 560 / scale + 272) << " " << (first ? "m" : "l") << "\n";
        first = false;
    }
    o << "h\n</path>\n";

    for (auto poly : polygons)
    {
        o << "<path layer=\"candidates\" stroke=\"black\" fill=\"red\" opacity=\"25%\" stroke-opacity=\"opaque\">";

        bool first = true;

        for (Point v : poly.poly)
        {
            o << ((v.x - xmin) * 560 / scale + 16) << " " << (v.y * 560 / scale + 272) << " " << (first ? "m" : "l") << "\n";
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

void Problem::visualizeSolution()
{

    std::string path = name + ".ipe";

    toIPE(path, container, candidates);
}

void Problem::roundItems()
{
    for (Item *item : items)
    {
        /*Path it = item->poly;

        Path ext;
        ext.push_back(Point(-1, -1));
        ext.push_back(Point(1, -1));
        ext.push_back(Point(1, 1));
        ext.push_back(Point(-1, 1));

        Path sum = Clipper2Lib::MinkowskiSum(it, ext, true)[0];*/

        Clipper2Lib::ClipperOffset offsetter;
        Paths result;
        offsetter.AddPath(item->poly, Clipper2Lib::JoinType::Miter, Clipper2Lib::EndType::Polygon);
        offsetter.Execute(6, result);

        // if (sum.is_clockwise_oriented()) {
        //     sum.reverse_orientation();
        // }

        item->poly = result[0];
    }
}