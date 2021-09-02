#include <iostream>
#include "problem.h"
double raw_epsilon_x = 0.0000003;
double raw_epsilon_y = 0.0000003;

double raw_extension_epsilon_x = 0.0000003;
double raw_extension_epsilon_y = 0.0000003;

void problem::readFile(char* inputFile)
{
	assert(raw_epsilon_x >= 0);
	file_name = string(inputFile);
	ifstream fp;
	fp.open(inputFile, std::ios::in);
	if (!fp.is_open())
	{
		perror("read input file fails");
	}

	string buff;
	char head;
	getline(fp, buff);
//#if defined GEOWORDLE_PRO || defined GEOWORDLE_PRO_RATIO
	char* str_text;
	while (!fp.eof())
	{
		if (buff.empty()) {
			getline(fp, buff);
			continue;
		}
		head = buff.at(0);
		if (head == 'l')
		{
			break;
		}
		getline(fp, buff);
	}
	str_text = strdup(buff.c_str());
//#endif



	while (!fp.eof())
	{
		if (buff.empty()) {
			getline(fp, buff);
			continue;
		}
		head = buff.at(0);
		if (head == 'p')
		{
			break;
		}
		getline(fp, buff);
	}
	// parse the p line
	char* str = strdup(buff.c_str());
	const char s[2] = " ";
	if (strtok(str, s)) {};
	width = atof(strtok(NULL, s));
	height = atof(strtok(NULL, s));
	pointNumber = atoi(strtok(NULL, s));
	colorNumber = atoi(strtok(NULL, s));
#ifdef GEOWORDLE_DRAW
	setColorPalette(colorNumber+2);
#endif
	assert(colorNumber > 1);
#ifdef GEOWORDLE_PRO_Tolerance
	if (tolerance_restriction) {
		int res_temp = ceil(tolerance_restriction_ratio* pointNumber);
		if (misscover_number < 0 ||(res_temp > 0 && res_temp < misscover_number)) {
			misscover_number = res_temp;
		}
		assert(misscover_number >= 0);
	}
#endif



//#if defined GEOWORDLE_PRO || defined GEOWORDLE_PRO_RATIO
	if (strtok(str_text, " ")) {};
	for(int i = 0; i< colorNumber; i++){
		char*  text = strtok(NULL, ",");
		//text[strlen(text) - 1] = 0;
		texts.push_back(text);
	}
//#endif

#if defined GEOWORDLE_PRO_RATIO || defined GEOWORDLE_PRO_Font_Size
#ifdef GEOWORDLE_DRAW
	cairo_surface_t* surface_temp = cairo_pdf_surface_create("test", width, height);
	cairo_t* cr_temp = cairo_create(surface_temp);
	for(const auto& text: texts)
	{
		text_ratios_map.push_back(find_ratio(cr_temp, text));
	}
#endif
#ifndef GEOWORDLE_DRAW
	for (const auto& text : texts)
	{
		text_ratios_map.push_back(make_tuple(0, 0, 1.0/strlen(text)));
	}
#endif
#endif
	set();

	double x, y, c;
	// parse the point lines
	for (unsigned int i = 0; i < pointNumber; i++)
	{
		getline(fp, buff);
		str = strdup(buff.c_str());
		x = atof(strtok(str, s));
		y = atof(strtok(NULL, s));
		c = atoi(strtok(NULL, s));
		add_raw_point(x, y, c);
	}
	//set_points();
#if defined GEOWORDLE_PRO_RATIO || defined GEOWORDLE_PRO_Font_Size
	//set_box_epsilons();
#endif
}
void problem::set() {
//	points.reserve(pointNumber);
	raw_points.reserve(pointNumber);
//	indexed_points.reserve(pointNumber);
//	x_table.pre_set(pointNumber);
//	y_table.pre_set(pointNumber);


	solution_bounds = vector<size_t>(colorNumber,0);
	// TODO: release the space later
	key_points.resize(colorNumber);
	size_t aver_points_per_color =(unsigned int)round((double)pointNumber / (double)colorNumber);
	for (size_t i = 0; i < colorNumber; i++) {
		key_points[i].reserve(aver_points_per_color);
	}
}
void problem::set_upper_bounds(vector<size_t>& upper_bound) {
	upper_bound = vector<size_t>(colorNumber+1, 0);
	for (const auto& p: raw_points) {
		upper_bound[p.c]++;
	}
	size_t accu = 0;
	size_t temp = 0;
	std::cout << std::endl;
	for (unsigned int i = 0; i <= colorNumber; i++) {
		temp = upper_bound[i];
		upper_bound[i] = accu;
		accu += temp;
	}
}
// pro texts
#if defined GEOWORDLE_PRO_RATIO || defined GEOWORDLE_PRO_Font_Size

void problem::set_box_epsilons() {
	box_x_epsilon = width;
	box_y_epsilon = height;
	double temp;

	// in the x direction
	sort(raw_points.begin(), raw_points.end(), sortbyY);
	double y_min = 0;
	temp = 0;
	for (const auto& e : raw_points)
	{
		temp = e.y() - y_min;
		assert(temp > 0);
		if (temp < box_y_epsilon)
		{

			box_y_epsilon = temp;
		}
		y_min = e.y();
	}


	//in the x direction
	sort(raw_points.begin(), raw_points.end());
	double x_min = 0;
	temp = 0;
	for (const auto& e : raw_points) 
	{
		temp = e.x() - x_min;
		assert(temp > 0);
		if (temp < box_x_epsilon)
		{
			box_x_epsilon = temp;
		}
		x_min = e.x();
	}
	assert(box_x_epsilon >= 0.0000005);
	assert(box_y_epsilon >= 0.0000005);
	box_x_epsilon = box_x_epsilon/2- raw_extension_epsilon_x;
	box_y_epsilon = box_y_epsilon/2 - raw_extension_epsilon_y;

	raw_epsilon_x = box_x_epsilon;
	raw_epsilon_y = box_y_epsilon;
}
#endif
void problem::set_lower_bounds() {

	bool set_key = true;
	for (size_t i = 0; i < pointNumber; i++){
		set_key = true;
		// check if p is a key points
		for (const auto& key_index : key_points[raw_points[i].c]) {
			// check if p could join a key point group
			if (is_mono(i, key_index)) {
				set_key = false;
				break;
			}		
		}
		if (set_key) {
			key_points[raw_points[i].c].push_back(i);
		}
	}
	//TODO: could clean up the key vectors now for current algorithms. 
	size_t count = 0;
	for (size_t i = 0; i < colorNumber; i++) {
		solution_bounds[i] = key_points[i].size();
		count += key_points[i].size();
	}
	solution_bound = count;
};
bool problem::is_mono(size_t i, size_t j) {
	if (i > j) swap(i, j);
	double y_min = raw_points[i].y();
	double y_max = raw_points[j].y();
	if (y_min > y_max) swap(y_min, y_max);
	size_t c = raw_points[i].c;
	for (size_t t = i; t < j; t++) {
		if (raw_points[t].c == c) continue;
		else {
			if (raw_points[t].y() <= y_max && raw_points[t].y() >= y_min) {
				return false;
			}
		}
	}
	assert(raw_points[i].c == raw_points[j].c);
	return true;
}
/*
void problem::set_points(){
	//x_table.set();
	//y_table.set();
	for (const auto& raw_point : raw_points) {
		points.push_back(colored_point(x_table.lookup_index(raw_point.x())
			, y_table.lookup_index(raw_point.y())
			, raw_point.c));
	}
	//sort(points.begin(), points.end());
	sort(raw_points.begin(), raw_points.end());
}
*/
#ifndef NDEBUG
void problem::debug() {
	assert(width > 0);
	assert(height>0);
	assert(colorNumber >1);
	assert(number_of_points() == pointNumber);
	// no two points are at the same position
	for (size_t i = 0; i < number_of_points()-1; i++) {
		assert(!(raw_points[i] == raw_points[i + 1]));
		assert(raw_points[i].c < colorNumber);
	
	}
	for (auto it = raw_points.begin(); it != raw_points.end(); ++it) {
		assert((*it).x() <= width);
		assert((*it).y() <= height);
	}
	/*
	for (auto it = points.begin(); it != points.end(); ++it) {
		assert((*it).x() <= points.size());
		assert((*it).y() <= points.size());
	}
	*/

};
#endif

void problem::test(char* inputFile, const char* appendix){
#ifdef GEOWORDLE_PRINT
	//print_points();
#endif
#ifndef NDEBUG
	//debug();
#endif
	// concatenation of file name
#ifdef GEOWORDLE_DRAW   // array to hold the result.

	const size_t first_after_slash_idx = string(inputFile).find_last_of("\\/") + 1;
	auto drawingFile = &inputFile[first_after_slash_idx];
	char result[400];
	strcpy(result, "D:/GIT/C++/geowordle_core_cmake/cairo_drawings/"); // copy string one into the result.
	strcat(result, drawingFile);
	strcat(result, "_problem_");// append string two to the result.
	strcat(result, appendix);
	strcat(result, ".pdf");
	cout << result << std::endl;

	startDrawing(getWidth(), getHeight(), result);
	assert(cr != nullptr);
	//draw(cr);
#endif
}

void problem::add_raw_point(double x, double y, color_t color) {
	raw_points.push_back(colored_point(x, y, color));
	//x_table.insert(x);
	//y_table.insert(y);
};



//!DRAW+++++++++++++++draw+++++++++++++++++++++++++
#ifdef GEOWORDLE_DRAW
void problem::draw(cairo_t* cr) {
	// draw points
	int i = 1;
	for (auto it = raw_points.begin(); it != raw_points.end(); ++it) {
		(*it).draw(cr, width, height, std::to_string(i).c_str());
		i++;
	}
#ifdef GEOWORDLE_PRO
	for (auto it = raw_points.begin(); it != raw_points.end(); ++it) {
		(*it).draw(cr, width, height, texts[it->c]);
	}
#endif
}
#endif
//!DRAW---------------draw-------------------------




//!PRINT+++++++++++++++PRINT+++++++++++++++++++++++++
#ifdef GEOWORDLE_PRINT
void problem::print_info()
{
	std::cout << "width: " << width << std::endl;
	std::cout << "height: " << height << std::endl;
	std::cout << "colorNumber: " << colorNumber << std::endl;
	std::cout << "pointNumber: " << number_of_points() << std::endl;

	std::cout << "** pre-computation" << std::endl;
	int i = 0;
	for (const auto& keys : key_points) {
		std::cout << i << ": " << "( " << solution_bounds[i] << ") ";
		for (auto key : keys) {
			std::cout << key << " ";
		}
		std::cout << std::endl;
		i++;
	}
#ifdef GEOWORDLE_PRO
	std::cout << "the texts: " << std::endl;
	for (const auto& e : texts) {
		std::cout << e<<std::endl;
	}
#endif
}

void problem::print() {
	print_info();
	print_points();
}
void problem::print_points()
{
	for (auto it = raw_points.begin(); it != raw_points.end(); ++it) {
		(*it).print();
	}
	std::cout << std::endl;
}
#endif
//!PRINT---------------PRINT-------------------------