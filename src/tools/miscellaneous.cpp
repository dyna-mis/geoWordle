#include "miscellaneous.h"
bool outputFlag;
string ito(int i)
{
    stringstream s;
    s << i;
    return s.str();
}
#ifdef GEOWORDLE_DRAW
void startDrawing(double width, double height, const char* fileName)
{
    surface = cairo_pdf_surface_create(fileName, width, height);
    std::cout << "width" << width << std::endl;
    std::cout << "height" << height << std::endl;
    surface = cairo_pdf_surface_create(fileName, width, height);
    //surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width * 1.2, height * 1.2);
    cr = cairo_create(surface);
    cairo_select_font_face(cr, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 10.0);
    cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);


    options = cairo_font_options_create();
    cairo_font_options_set_antialias(options,CAIRO_ANTIALIAS_BEST);
    cairo_set_font_options(cr,options);
    cairo_set_antialias(cr,CAIRO_ANTIALIAS_BEST);
    cairo_set_line_width(cr, 2.5);
   // cairo_rectangle(cr, width * 0.1, height * 0.1, width, height);
    cairo_stroke(cr);
}
void finishDrawing()
{
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    cairo_font_options_destroy(options);
}
#endif


#ifdef GEOWORDLE_CGAL
bool check_inside(Point_2 pt, Polygon_2 pgn, K traits)
{
    cout << "The point " << pt;
    switch (CGAL::bounded_side_2(pgn.vertices_begin(), pgn.vertices_end(), pt, traits)) {
    case CGAL::ON_BOUNDED_SIDE:
        cout << " is inside the polygon.\n";
        return true;
    case CGAL::ON_BOUNDARY:
        cout << " is on the polygon boundary.\n";
        return true;
    case CGAL::ON_UNBOUNDED_SIDE:
        cout << " is outside the polygon.\n";
        return false;
    default:
        assert(false);
        return false;
    }

}
Polygon_with_Points::Polygon_with_Points()
{
    vector<Point_2> corners = { Point_2(100, 100), Point_2(100, 400),
        Point_2(200, 500), Point_2(300, 400), Point_2(300, 200), Point_2(500, 200),
        Point_2(500, 400), Point_2(600, 500), Point_2(700, 400), Point_2(700, 100) };
    polygon = Polygon_2(corners.begin(), corners.end());

    // check if the polygon is simple.
    cout << "The polygon is " << (polygon.is_simple() ? "" : "not ") << "simple." << endl;
    // check if the polygon is convex
    cout << "The polygon is " << (polygon.is_convex() ? "" : "not ") << "convex." << endl;
    points = { Point_2(200,300) };
    checkInside();
#ifdef GEOWORDLE_DRAW
    CGAL::draw(polygon);
#endif
};

Polygon_with_Points::Polygon_with_Points(vector<Point_2> cornersP, vector<Point_2> pointsP)
{
    polygon = Polygon_2(cornersP.begin(), cornersP.end());
    points = pointsP;

};
//++++++++++++++++++++++++++++debug+++++++++++++++++++++++++++++++++
void Polygon_with_Points::checkInside() {
    for (auto point : points) {
        assert(check_inside(point, polygon, K()));
    }
}
//----------------------------debug---------------------------------
#endif


size_t getRange(size_t value, const std::vector<size_t>& incr_vector) {
    size_t s = incr_vector.size();
    for (unsigned int i = 0; i < s; i++) {
        if (value < incr_vector[i]) {
            return i - 1;
        }
        else {
            continue;
        }
    }
    assert(false);
    return -1;
}



// customized bi-directed hashing table. 
/*
void lookupTable::pre_set(size_t number_of_points){
    coordinates.reserve(number_of_points+1);
    indices.reserve(number_of_points+1);
    coordinates.push_back(0);
};
// insert a new labelID
void  lookupTable::insert(double co) {
    coordinates.push_back(co);
};
void lookupTable::set() {
    sort(coordinates.begin(), coordinates.end());
    for (unsigned int i = 0; i < coordinates.size(); i++) {
        indices.insert({coordinates[i],i});
    }

}
// lookup the coordinate of one index
double lookupTable::lookup_co(size_t index){
    assert(index < coordinates.size());
    return coordinates[index];
};


// lookup the index of one coordinate
size_t lookupTable::lookup_index(double co){
    unordered_map<double, size_t>::const_iterator it = indices.find(co);
    assert(it != indices.end());
    return it->second;
};
// debug code
#ifndef NDEBUG
void lookupTable::debug() {
    assert(indices.size() == coordinates.size());
    for (auto c_i_pair : indices) {
        assert(lookup_index(c_i_pair.first) == c_i_pair.second);
    }
    for (int i = 1; i <(int)coordinates.size(); i++) {
        int coordinate = coordinates[i];
        assert(lookup_co(lookup_index(coordinate)) == coordinate);
    }
};
#endif
// print the look_up_table
void lookupTable::print() {
    cout << "indices:  " << indices.size() << endl;
    for (auto vID : indices) {
        cout << vID.first << " " << vID.second << endl;
    }
    cout << "coordinates: " << coordinates.size() << endl;
    for (int i = 1; i < coordinates.size(); i++) {
        cout << i << ": " << coordinates[i] << endl;
    }
};
* /
/*
#ifndef NDEBUG
void test_Assertion() {
    std::cout << "GeoWordle" << std::endl;

    assert(1 == 0, "message: value of i must be greater than 9");
};
#endif
*/


//print the manual of the program
void printInitUsage() {
    cout << "\n--------geoWordle_CORE, a geometric rectangle cover solver by " << BLUE_t << "TUWIEN" << RESET_t << " -----------------\n";
    printf("Authors: Sujoy Bhore, Robert Ganian, Guangping Li, Martin Nöllenburg and Jules Wulms\n");
    printf("Human-centered Algorithm Engineering: Graph and Map Visualization\n");
    printf("Research Project supported by the Austrian Science Fund(FWF), grant P 31119\n");
    printf("https://www.ac.tuwien.ac.at/research/humalgo/\n");

    printf("\n-------------------Usage--------------------------------------------------------\n");
    printf("./geoWordle   <instance> [options]\n");

    printf("\n-------------------Options------------------------------------------------------\n");
    printf("   --output, -o : output the solution\n");
    printf("   --help, -h : output this help\n");

    printf("   --model, -m : model in use (see options below)\n");
    printf("   --algorithm, -a : algorithm in use (see options below)\n");

    printf("---------------------------------------------------------------------------------\n");
    /*
    cout << CYAN_t;
    printf("\n-------------------model options-----------------------------------------------\n");
    printf("  CC: stands for Class Cover, which covers all the points in the class and prevent to cover any obstacle points\n");
    printf("  MC: stands for Monochromatic Cover, which covers all the pints with boxes of consisting colors\n");
    printf("  DCC: stands for Disjoint Class Cover, which is a valid class cover with a set of pairwise disjoint boxes\n");
    printf("  DMC: stands for Disjoint Monochromatic Cover, which is a valid monochromatic cover with pairwise disjoint boxes\n");
    printf("---------------------------------------------------------------------------------\n");
    */
    // add more algorithm options here
    printf("---------------------------------------------------------------------------------\n");
    cout << GREEN_t << "\n-------------------algorithm options-------------------------------------------\n";
    // exact solver
    printf("   ILP : a Integer Linear Programming Solver \n");
    // heuristics
    printf("   CELL: a Voronoi-Diagram based Solver\n");

    printf("---------------------------------------------------------------------------------\n");

    printf("---------------------------------------------------------------------------------\n");
    cout << GREEN_t << "\n-------------------comments-------------------------------------------\n";
    printf("   More technical details in the file \"docs/user_guide.pdf\" \n");
    //TODO: adding references
    printf("\n");

    printf("---------------------------------------------------------------------------------\n");
    std::cout << RESET_t;
};

void parseInitOptions(int argc, char* argv[]) {
    cxxopts::Options options("geoWordle", "Geometric Covering");

    options.add_options()
        ("a,algorithm", "Algorithm Option", cxxopts::value<std::string>())
        ("h,help", "Print usage")
        ("o,output", "Print Solution")
        ("filename", "input file", cxxopts::value<std::string>())
        ("t", "tmp_dictionary", cxxopts::value<std::string>()->default_value("D:/GIT/C++/geowordle_core_cmake"))
        ("d", "dictionary", cxxopts::value<std::string>()->default_value("D:/GIT/C++/geowordle_core_cmake"))
#ifdef GEOWORDLE_PRO_RATIO
        ("l", "lower_bound_ratio", cxxopts::value<double>()->default_value("0.5"))
        ("u", "upper_bound_ratio", cxxopts::value<double>()->default_value("2"))
#endif
#ifdef GEOWORDLE_PRO_Tolerance
        ("m", "miss-cover tolerance ratio", cxxopts::value<double>()->default_value("0.75"))
        // relative miss ratio
        ("g", "miss-cover global ratio", cxxopts::value<double>()->default_value("-0.05"))
        // absolute miss number
        ("n", "miss-cover tolerance number", cxxopts::value<int>()->default_value("-1"))
#endif

#ifdef	GEOWORDLE_PRO_Font_Size
        ("f, font", "minimum font size", cxxopts::value<double>()->default_value("-1"))
#endif
        ;
    options.parse_positional({ "filename" });
    options.allow_unrecognised_options();
    auto result = options.parse(argc, argv);
    try {
        file = strdup(result["filename"].as<std::string>().c_str());
        string outFile(file);
        std::string key = "/";
        std::size_t found = outFile.rfind(key);
        outFile = outFile.substr(found + 1);
        measures.addElement("info", "file", outFile);
    }
    catch (const std::exception& e) {
        std::cerr << "Input file missing" << std::endl;
        printInitUsage();
        exit(0);
    }

    if (result.count("help"))
    {
        printInitUsage();
        exit(0);
    }
    result_folder = result["d"].as<std::string>();
    tmp_dictionary = result["t"].as<std::string>();
#ifdef GEOWORDLE_PRO_RATIO
    if (result.count("u"))
    {
        ratio_u = result["u"].as<double>();
        measures.addElement("info", "ratio_u", std::to_string(ratio_u));
    }
    if (result.count("l"))
    {
        ratio_l = result["l"].as<double>();
        measures.addElement("info", "ratio_l", std::to_string(ratio_l));
    }
#endif
#ifdef GEOWORDLE_PRO_Tolerance

    if (result.count("g") || result.count("n"))
    {
        tolerance_restriction = true;
    }
    if (result.count("m"))
    {
        ratio_to = result["m"].as<double>();
        measures.addElement("info", "tolerance_ratio", std::to_string(ratio_to));
    }
    if (result.count("g"))
    {
        tolerance_restriction_ratio = result["g"].as<double>();
        measures.addElement("info", "tolerance_ratio_global", std::to_string(tolerance_restriction_ratio));
    }
    if (result.count("n"))
    {
        misscover_number = result["n"].as<int>();
        measures.addElement("info", "tolerance_misscover_number", std::to_string(misscover_number));
    }
#endif


#ifdef	GEOWORDLE_PRO_Font_Size

    //TODO: real height = min{width, height}/1000 * font_size
    if (result.count("f")) {
        min_font_size = result["f"].as<double>();
        if (min_font_size > 0) font_siz_restriction = true;
        measures.addElement("info", "min_font_size", std::to_string(min_font_size));

    }
#endif

    if (result.count("algorithm")) {
        algoType = result["algorithm"].as<std::string>();
        measures.addElement("info", "algo_type", algoType);
    }
    if (result.count("output")) {
        outputFlag = true;
    }

};


void outputMeasure(const char* append) {
    string outFile(file);
    std::string key = "/";
    std::size_t found = outFile.rfind(key);
    outFile = outFile.substr(found + 1);
    string appendix;
    appendix += "-";
    appendix += algoType;

#ifdef GEOWORDLE_PRO_RATIO
    // ratio (lower, upper)
    appendix += "-";
    appendix += std::to_string(ratio_u);
    appendix += "-";
    appendix += std::to_string(ratio_l);
#endif

#ifdef GEOWORDLE_PRO_Tolerance
    appendix += "-";
    appendix += std::to_string(ratio_to);
    appendix += "-";
    appendix += std::to_string(tolerance_restriction_ratio);
    appendix += "-";
    appendix += std::to_string(misscover_number);
    // tolerance(ratio, exact number)s
#endif

#ifdef GEOWORDLE_PRO_Font_Size
    appendix += "-";
    appendix += std::to_string(min_font_size);
#endif
    appendix += "-";
    appendix += excute_name;
    appendix += append;
    measures.output(result_folder.c_str(), appendix.c_str(), outFile.c_str());
}