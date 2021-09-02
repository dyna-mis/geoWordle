
#include "solver.h"
SolveStatus solver::test(problem& P, jsonM& measures) {
	SolveStatus solved = solve(P, measures);
	if (solved == SolveStatus::Unsolved) {
		return solved;
	} 
#ifndef NDEBUG
	//debugSetting(P);
	//debug(P);
#endif
#ifdef GEOWORDLE_PRINT
	//printSolution();
#endif
#ifdef GEOWORDLE_DRAW
	assert(cr != nullptr);
	drawSolution(cr, P);
	//finishDrawing();
#endif
	return solved;
	// output 
}

void solver::set_candidate_set(problem& P, jsonM& measures) {
	sort(P.raw_points.begin(), P.raw_points.end());
	assert(raw_epsilon_x >= 0);
	// we use deque
	//candidates_set.reserve(P.pointNumber);

	size_t temp_size = P.raw_points.size();
	for (size_t i = 0; i < temp_size; i++) {
		//std::cout << "set candidate  i : " << i << std::endl;
#if defined GEOWORDLE_PRO_RATIO ||defined GEOWORDLE_PRO_Font_Size
		size_t j = i+1;
#else
		size_t j = i;
#endif
		for (; j < temp_size; j++) {
			//std::cout << i << " with  set candidate j : " << j << std::endl;
#ifndef GEOWORDLE_PRO_Tolerance
			if (P.raw_points[i].c != P.raw_points[j].c) continue;
#endif
			deque<tuple<colored_box, size_t, double>> candidates_set_temp;
#ifdef GEOWORDLE_PRO_RATIO
			get_points_stripe(i, j, P, candidates_set_temp);

#else
			get_points_stripe(i, j, P, candidates_set_temp);
#endif
			candidates_set.insert(candidates_set.end(), candidates_set_temp.begin(), candidates_set_temp.end());

#ifdef GEOWORDLE_PRO_Font_Size
#ifndef NDEBUG
			debugSolution_fonts_with_auxilaries(P);
#endif
#endif

		}
	}

#ifdef GEOWORDLE_PRO_RATIO
#ifndef NDEBUG
	debugSolution_ratios_with_auxilaries(P);
#endif
#endif
#ifndef NDEBUG
	debugSolution_basic_with_auxilaries(P);
#endif

#ifdef GEOWORDLE_PRO_Font_Size
#ifndef NDEBUG
	debugSolution_fonts_with_auxilaries(P);
#endif
#endif


#if defined GEOWORDLE_PRO_Font_Size
	if (font_siz_restriction) {
		for (size_t i = 0; i < temp_size; i++)
		{
			add_singleton_candidates_font(P, i);
		}

	}
#endif


#ifndef GEOWORDLE_PRO_Font_Size
#ifdef GEOWORDLE_PRO_RATIO 
	for (size_t i = 0; i < temp_size; i++) 
	{
		bool horizontal = (P.box_x_epsilon > P.box_y_epsilon);
		double w = horizontal ? P.box_x_epsilon : P.box_y_epsilon;
		double h = w * get<2>(P.text_ratios_map[P.raw_points[i].c]);
		if (!horizontal) swap(w, h);
		assert(horizontal == (w >= h));
		colored_box rect= colored_box(
			P.raw_points[i].x()-w
			, P.raw_points[i].x()+ w,
			P.raw_points[i].y() - h
			, P.raw_points[i].y() + h,
			P.raw_points[i].c);
		assert(rect.geo_is_cover(P.raw_points[i]));
		candidates_set.push_back(tuple<colored_box, double , double>(rect, 1, 1));
	}
#endif
#endif



#ifndef NDEBUG
	debugSolution_basic_with_auxilaries(P);
#endif


#ifdef GEOWORDLE_PRO_RATIO
#ifndef NDEBUG
	debugSolution_ratios_with_auxilaries(P);
#endif
#endif

#ifdef GEOWORDLE_PRO_Font_Size
#ifndef NDEBUG
	debugSolution_fonts_with_auxilaries(P);
#endif
#endif

	candidates_set.shrink_to_fit();
	//sort candidate set
	add_penalty(P);
	std::sort(candidates_set.begin(), candidates_set.end(), sortbysec);
	measures.addElement("info", "number_candidates", to_string(candidates_set_size()));
}




#ifdef GEOWORDLE_PRO_Tolerance

//TODO: adapt it to tolerance version

void solver::get_points_stripe(size_t start_back,
	size_t start_for, problem& P, deque<tuple<colored_box, size_t, double>>& candidates_set_temp) {
#ifdef GEOWORDLE_PRO_Font_Size
#ifndef NDEBUG
	debugSolution_fonts_with_auxilaries_temp(P, candidates_set_temp);
#endif
#endif

	assert(raw_epsilon_x >= 0);

	colored_point p_upper = P.raw_points[start_back];
	colored_point p_lower = P.raw_points[start_for];

	if (p_upper.y() < p_lower.y()) {
		swap(p_upper, p_lower);
	}

	size_t temp_size = P.raw_points.size();
	vector<cover_records> seed_cover_records;
	vector<colored_box> candidates_set_seed;
#ifdef GEOWORDLE_PRO_Extend
	vector<size_t> candidates_set_seed_left;
#endif 
	// core
#ifdef GEOWORDLE_PRO_RATIO
//TODO; after debug, remove 
	double c = p_upper.c;
	double ratio = get<2>(P.text_ratios_map[c]);
	double h = get<0>(P.text_ratios_map[c]);
	double w = get<1>(P.text_ratios_map[c]);

	double ratio_upper = ratio_u * ratio;
	double ratio_lower = ratio_l * ratio;
#endif
#ifdef GEOWORDLE_PRO_RATIO
	colored_box rect = colored_box(P.raw_points[start_back].x() - raw_epsilon_x, P.raw_points[start_for].x() + raw_epsilon_x
		, p_lower.y() - raw_epsilon_y, p_upper.y() + raw_epsilon_y, p_upper.c);
#else
	colored_box rect = colored_box(P.raw_points[start_back].x(), P.raw_points[start_for].x()
		, p_lower.y(), p_upper.y(), p_upper.c);
#endif
	cover_records record = cover_records(colorNumber);
	// check valid
	for (size_t i = start_back; i <= start_for; i++) {
		if (P.raw_points[i].y() > rect.ymax()) continue;
		if (P.raw_points[i].y() < rect.ymin()) continue;
		size_t c = P.raw_points[i].c;
#ifdef GEOWORDLE_PRINT
		//P.raw_points[i].print();
#endif
		record.increase(c);
	}
	// lamda(variants)
	// count^2/ area
	// count * (s/l)
	// post process  ---> 
	// TODO: check ratio and break 
	double tol = record.ratio();
	rect.c = record.domi_color;
	bool valid_misscover = true;
	if (tolerance_restriction) {
		int local_miss_number = record.miss_cover_number();
		if (local_miss_number <= misscover_number)
		{

			seed_cover_records.push_back(record);
			candidates_set_seed.push_back(rect);
#ifdef GEOWORDLE_PRO_Extend
			candidates_set_seed_left.push_back(start_back);
#endif 
		}
		else {
			valid_misscover = false;
		}
	
	}
	else {
		seed_cover_records.push_back(record);
		candidates_set_seed.push_back(rect);
#ifdef GEOWORDLE_PRO_Extend
		candidates_set_seed_left.push_back(start_back);
#endif 
	}


#ifdef GEOWORDLE_PRO_RATIO // width-height-ratio
	double r = rect.ratio();
	ratio_lower = ratio_l * get<2>(P.text_ratios_map[rect.c]);
	ratio_upper = ratio_u * get<2>(P.text_ratios_map[rect.c]);
	if (rect.getWidth() > rect.getHeight() && r < ratio_lower)
	{
#ifdef GEOWORDLE_PRO_Extend
			deque<colored_box> extension_temp;
			add_invalid_candidates(P, ratio_upper, ratio_lower,
				start_back, start_for, rect, extension_temp);
			if (extension_temp.size() == 0) return;
			else {
				if (tol >= ratio_to && valid_misscover) {
					for (const auto& e : extension_temp) {
#ifdef GEOWORDLE_PRO_Font_Size
						if (e.get_text_height() >= min_font_size) {
							assert(e.xmin() <= rect.xmin());
							assert(e.xmax() >= rect.xmax());
							candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, record.sum, record.sum));
						}
#else
						candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, record.sum, record.sum));
#endif

					}
				}
			}

#else
		return;
#endif
	}
#ifdef GEOWORDLE_PRO_RATIO
#ifndef NDEBUG
	debugRatio(P, candidates_set_temp);
#endif
#endif
#ifdef GEOWORDLE_PRO_Extend
	if (r >= ratio_lower && r <= ratio_upper && tol >= ratio_to && valid_misscover)
	{
		rect.c = record.domi_color;
		if (tolerance_restriction) {
			assert(record.miss_cover_number() <= misscover_number);
		}
#ifdef GEOWORDLE_PRO_Font_Size
		if (rect.get_text_height() >= min_font_size) {
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(rect, record.sum, record.sum));
		}
#else
		candidates_set_temp.push_back(tuple<colored_box, size_t, double>(rect, record.sum, record.sum));
#endif

	}
	else if(tol >= ratio_to && valid_misscover)
	{
		deque<colored_box> extension_temp;
		add_invalid_candidates(P, ratio_upper, ratio_lower,
			start_back, start_for, rect, extension_temp);
		for (const auto& e : extension_temp) {
			if (tolerance_restriction) {
				assert(record.miss_cover_number() <= misscover_number);
			}
#ifdef GEOWORDLE_PRO_Font_Size
			if (e.get_text_height() >= min_font_size) {
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, record.sum, record.sum));
			}
#else
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, record.sum, record.sum));
#endif

		}

	}
#ifdef GEOWORDLE_PRO_RATIO
#ifndef NDEBUG
	debugRatio(P, candidates_set_temp);
#endif
#endif

#else
	if (r >= ratio_lower && r <= ratio_upper && tol >= ratio_to && valid_misscover)
	{
		rect.c = record.domi_color;
#ifdef GEOWORDLE_PRO_Font_Size
		if (e.get_text_height() >= min_font_size) {
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(rect, record.sum, record.sum));
			}
#else
		candidates_set_temp.push_back(tuple<colored_box, size_t, double>(rect, record.sum, record.sum));
#endif

	}
#endif
#ifdef GEOWORDLE_PRO_RATIO
#ifndef NDEBUG
	debugRatio(P, candidates_set_temp);
#endif
#endif

#else //no width-height-ratio

	if (tol >= ratio_to && valid_misscover)
	{
		rect.c = record.domi_color;
#ifdef GEOWORDLE_PRO_Font_Size
		if (e.get_text_height() >= min_font_size) {
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(rect, record.sum, record.sum));
		}
#else
		candidates_set_temp.push_back(tuple<colored_box, size_t, double>(rect, record.sum, record.sum));
#endif

	}
#endif
	//colored_box<int> extended = colored_box<int>(rect);


#ifdef GEOWORDLE_PRO_RATIO
#ifndef NDEBUG
	debugRatio(P, candidates_set_temp);
#endif
#endif

	cover_records record_extend = record;

	colored_box extended = colored_box(rect.xmin(), rect.xmax()
		, rect.ymin(), rect.ymax(), rect.c);

	for (int i = start_back - 1; i >= 0; i--) {
		if (P.raw_points[i].y() > rect.ymax()) continue;
		if (P.raw_points[i].y() < rect.ymin()) continue;

		size_t c = P.raw_points[i].c;
		record_extend.increase(c);

		extended = extended.extend_stripe(P.raw_points[i]);
		extended.c = record_extend.domi_color;

		tol = record_extend.ratio();
		valid_misscover = true;
		if (tolerance_restriction && record_extend.miss_cover_number() > misscover_number) {
			valid_misscover = false;
		}
#ifdef GEOWORDLE_PRO_RATIO
		r = extended.ratio();
		ratio_lower = ratio_l * get<2>(P.text_ratios_map[extended.c]);
		ratio_upper = ratio_u * get<2>(P.text_ratios_map[extended.c]);

#ifdef GEOWORDLE_PRO_Extend
		if (extended.getWidth() > extended.getHeight() && r < ratio_lower) {

			deque<colored_box> extension_temp_left;
			add_invalid_candidates(P, ratio_upper, ratio_lower,
				i, start_for, extended, extension_temp_left);
			if (extension_temp_left.size() == 0 || !valid_misscover) break;
			else {
				if (tol >= ratio_to && valid_misscover)
				{
					for (const auto& e : extension_temp_left) {
						if (tolerance_restriction) {
							assert(record_extend.miss_cover_number() <= misscover_number);
						}
#ifdef GEOWORDLE_PRO_Font_Size
						if (e.get_text_height() >= min_font_size) {
							candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, record_extend.sum, record_extend.sum));
						}
#else
						candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, record_extend.sum, record_extend.sum));
#endif
						
					}
				}
			}

		};
#else
		if (rect.getWidth() > rect.getHeight() && r < ratio_lower) break;

#endif

#ifdef GEOWORDLE_PRO_Extend
		if (r >= ratio_lower && r <= ratio_upper && tol >= ratio_to && valid_misscover)
		{
			if (tolerance_restriction) {
				assert(record_extend.miss_cover_number() <= misscover_number);
			}
#ifdef GEOWORDLE_PRO_Font_Size
			if (extended.get_text_height() >= min_font_size) {
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended,
					record_extend.sum, record_extend.sum));
			}
#else
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended,
				record_extend.sum, record_extend.sum));
#endif


		}
		else if (tol >= ratio_to && valid_misscover)
		{

			deque<colored_box> extension_temp_left;
			add_invalid_candidates(P, ratio_upper, ratio_lower,
				i, start_for, extended, extension_temp_left);
			for (const auto& e : extension_temp_left) {
				if (tolerance_restriction) {
					assert(record_extend.miss_cover_number() <= misscover_number);
				}
#ifdef GEOWORDLE_PRO_Font_Size
				if (e.get_text_height() >= min_font_size) {
					candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, record_extend.sum, record_extend.sum));
				}
#else
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, record_extend.sum, record_extend.sum));
#endif


			}
		}

#else
		if (r >= ratio_lower && r <= ratio_upper && tol >= ratio_to&& valid_misscover)
		{
#ifdef GEOWORDLE_PRO_Font_Size
			if (e.get_text_height() >= min_font_size) {
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended,
					record_extend.sum, record_extend.sum));
			}
#else
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended,
				record_extend.sum, record_extend.sum));
#endif

		}
#endif
#ifdef GEOWORDLE_PRO_RATIO
#ifndef NDEBUG
			//debugRatio(P, candidates_set_temp);
#endif
#endif
#else 
		if (tol >= ratio_to && valid_misscover) {
			extended.c = record_extend.domi_color;
#ifdef GEOWORDLE_PRO_Font_Size
			if (extended.get_text_height() >= min_font_size) {
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended,
					record_extend.sum, record_extend.sum));
			}
#else
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended,
				record_extend.sum, record_extend.sum));
#endif

		}

#endif
		if(valid_misscover){
			seed_cover_records.push_back(record_extend);
			candidates_set_seed.push_back(extended);

#ifdef GEOWORDLE_PRO_Extend
			candidates_set_seed_left.push_back(i);
#endif 
		}
	}




#ifdef GEOWORDLE_PRO_RATIO
#ifndef NDEBUG
	//debugRatio(P, candidates_set_temp);
#endif
#endif
#ifdef GEOWORDLE_PRO_Font_Size
#ifndef NDEBUG
	debugSolution_fonts_with_auxilaries_temp(P, candidates_set_temp);
#endif
#endif
	//TODO: traverse the current boxes
	size_t left_rectangles_size = candidates_set_seed.size();

	cover_records extended_right = cover_records(colorNumber);
	for (int i = start_for + 1; i < temp_size; i++) {
		if (P.raw_points[i].y() > rect.ymax()) continue;
		if (P.raw_points[i].y() < rect.ymin()) continue;
		
		extended_right.increase(P.raw_points[i].c);


		for (size_t j = 0; j < left_rectangles_size; j++) {
			valid_misscover = true;
			cover_records total_record = extended_right.merge(seed_cover_records[j]);
			extended = candidates_set_seed[j].extend_stripe(P.raw_points[i]);
			extended.c = total_record.domi_color;
			tol = total_record.ratio();
			if (tolerance_restriction) {
				valid_misscover = (total_record.miss_cover_number() < misscover_number);
			}
			if (!valid_misscover) break;
#ifdef GEOWORDLE_PRO_RATIO
			r = extended.ratio();
			ratio_lower = ratio_l * get<2>(P.text_ratios_map[extended.c]);
			ratio_upper = ratio_u * get<2>(P.text_ratios_map[extended.c]);

#ifdef GEOWORDLE_PRO_Extend
			if (extended.getWidth() > extended.getHeight() && r < ratio_lower) {
				if (tol >= ratio_to && valid_misscover) {
					deque<colored_box> extension_temp_right;
					add_invalid_candidates(P, ratio_upper, ratio_lower,
						candidates_set_seed_left[j], i, extended, extension_temp_right);
					if (extension_temp_right.size() == 0) break;
					else {
						for (const auto& e : extension_temp_right) {
							if (tolerance_restriction) {
								assert(total_record.miss_cover_number() <= misscover_number);
							}
#ifdef GEOWORDLE_PRO_Font_Size
							if (e.get_text_height() >= min_font_size) {
								candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, total_record.sum, total_record.sum));
							}
#else
							candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, total_record.sum, total_record.sum));
#endif


						}
					}
				}

			}
#else
			if (extended.getWidth() > extended.getHeight() && r < ratio_lower) break;
#endif
#ifdef GEOWORDLE_PRO_Extend
			if (r >= ratio_lower && r <= ratio_upper && tol >= ratio_to && valid_misscover)
			{
				if (tolerance_restriction) {
					assert(total_record.miss_cover_number() <= misscover_number);
				}
#ifdef GEOWORDLE_PRO_Font_Size
				if (extended.get_text_height() >= min_font_size) {
					candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, total_record.sum, total_record.sum));
				}
#else
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, total_record.sum, total_record.sum));
#endif

			}
			else if (tol >= ratio_to && valid_misscover)
			{
				
				deque<colored_box> extension_temp_right;
				add_invalid_candidates(P, ratio_upper, ratio_lower,
					candidates_set_seed_left[j], i, extended, extension_temp_right);
				if (extension_temp_right.size() == 0) break;
				else {
					for (const auto& e : extension_temp_right) {
						if (tolerance_restriction) {
							assert(total_record.miss_cover_number() <= misscover_number);
						}
#ifdef GEOWORDLE_PRO_Font_Size
						if (e.get_text_height() >= min_font_size) {
							candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, total_record.sum, total_record.sum));
						}
#else
						candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, total_record.sum, total_record.sum));
#endif

					}
				}
			}
#else
			if (r >= ratio_lower && r <= ratio_upper && tol >= ratio_to && valid_misscover)
			{
#ifdef GEOWORDLE_PRO_Font_Size
				if (extended.get_text_height() >= min_font_size) {
					candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, total_record.sum, total_record.sum));
		}
#else
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, total_record.sum, total_record.sum));
#endif

			}
#endif








#else
			if (tol >= ratio_to)
			{
				extended.c = total_record.domi_color;
#ifdef GEOWORDLE_PRO_Font_Size
				if (extended.get_text_height() >= min_font_size) {
					candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, total_record.sum, total_record.sum));
				}
#else
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, total_record.sum, total_record.sum));
#endif

			}
#endif
		}
	}
#ifdef GEOWORDLE_PRO_RATIO
#ifndef NDEBUG
	//debugRatio(P, candidates_set_temp);
#endif
#endif
#ifdef GEOWORDLE_PRO_Font_Size
#ifndef NDEBUG
	debugSolution_fonts_with_auxilaries_temp(P, candidates_set_temp);
#endif
#endif
}

#else


//TODO: candidate_set_temp
void solver::get_points_stripe(size_t start_back,
	size_t start_for, problem& P, deque<tuple<colored_box, size_t, double>>& candidates_set_temp) {
#ifdef GEOWORDLE_PRO_Font_Size
#ifndef NDEBUG
	debugSolution_fonts_with_auxilaries_temp(P, candidates_set_temp);
#endif
#endif

	colored_point p_upper = P.raw_points[start_back];
	colored_point p_lower = P.raw_points[start_for];
	assert(p_upper.c == p_lower.c);
	if (p_upper.y() < p_lower.y()) {
		swap(p_upper, p_lower);
	}

	size_t temp_size = P.raw_points.size();
	// core
#ifdef GEOWORDLE_PRO_RATIO
//TODO; after debug, remove 
	double c = p_upper.c;
	double ratio = get<2>(P.text_ratios_map[c]);
	double h = get<0>(P.text_ratios_map[c]);
	double w = get<1>(P.text_ratios_map[c]);

	vector<tuple<colored_box, size_t, double>> candidates_set_seed;
#ifdef GEOWORDLE_PRO_Extend
	vector<size_t> candidates_set_seed_left;
#endif 
	double ratio_upper = ratio_u * ratio;
	double ratio_lower = ratio_l * ratio;
#endif
#ifdef GEOWORDLE_PRO_RATIO
	colored_box rect = colored_box(P.raw_points[start_back].x() - raw_epsilon_x, P.raw_points[start_for].x() + raw_epsilon_x
		, p_lower.y() - raw_epsilon_y, p_upper.y() + raw_epsilon_y, p_upper.c);
#else
	colored_box rect = colored_box(P.raw_points[start_back].x(), P.raw_points[start_for].x()
		, p_lower.y(), p_upper.y(), p_upper.c);
#endif

	size_t count = 0;
	// check valid
	for (size_t i = start_back; i <= start_for; i++) {
		if (P.raw_points[i].y() > rect.ymax()) continue;
		if (P.raw_points[i].y() < rect.ymin()) continue;
		if (P.raw_points[i].c != rect.c) return;
		count++;
	}
	// lamda(variants)
	// count^2/ area
	// count * (s/l)
	// post process  ---> 
	// TODO: check ratio and break 
#ifdef GEOWORDLE_PRO_RATIO
	double r = rect.ratio();
	if (rect.getWidth() > rect.getHeight() && r < ratio_lower) 
	{
	#ifdef GEOWORDLE_PRO_Extend
		deque<colored_box> extension_temp;
		add_invalid_candidates(P, ratio_upper, ratio_lower,
			start_back, start_for, rect,extension_temp);
		if (extension_temp.size() == 0) return;
		else {
			for (const auto& e : extension_temp) {

#ifdef GEOWORDLE_PRO_Font_Size
				if (e.get_text_height() >= min_font_size) {
					candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, count, count));
				}
#else
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, count, count));
#endif


			}
		}

	#else
		return;
	#endif
	}
	candidates_set_seed.push_back(tuple<colored_box, size_t, double>(rect, count, count));
#ifdef GEOWORDLE_PRO_Extend
	candidates_set_seed_left.push_back(start_back);
#endif 
#ifdef GEOWORDLE_PRO_Extend
	if (r >= ratio_lower && r <= ratio_upper)
	{
#ifdef GEOWORDLE_PRO_Font_Size
		if (rect.get_text_height() >= min_font_size) {
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(rect, count, count));
		}
#else
		candidates_set_temp.push_back(tuple<colored_box, size_t, double>(rect, count, count));
#endif

	}
	else 
	{
		deque<colored_box> extension_temp;
		add_invalid_candidates(P, ratio_upper, ratio_lower,
			start_back, start_for, rect, extension_temp);
		for (const auto& e : extension_temp) {
#ifdef GEOWORDLE_PRO_Font_Size
			if (e.get_text_height() >= min_font_size) {
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, count, count));
			}
#else
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, count, count));
#endif

		}

	}

#else
	if (r >= ratio_lower && r <= ratio_upper)
	{
#ifdef GEOWORDLE_PRO_Font_Size
		if (rect.get_text_height() >= min_font_size) {
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(rect, count, count));
}
#else
		candidates_set_temp.push_back(tuple<colored_box, size_t, double>(rect, count, count));
#endif
	}
#endif
#else
#ifdef GEOWORDLE_PRO_Font_Size
	if (rect.get_text_height() >= min_font_size) {
		candidates_set_temp.push_back(tuple<colored_box, size_t, double>(rect, count, count));
	}
#else
	candidates_set_temp.push_back(tuple<colored_box, size_t, double>(rect, count, count));
#endif
#endif
	//colored_box<int> extended = colored_box<int>(rect);


#ifdef GEOWORDLE_PRO_Font_Size
#ifndef NDEBUG
	debugSolution_fonts_with_auxilaries_temp(P, candidates_set_temp);
#endif
#endif

#ifdef GEOWORDLE_PRO_RATIO
	colored_box extended = colored_box(rect.xmin(), rect.xmax()
		, rect.ymin(), rect.ymax(), rect.c);
#else
	colored_box extended = colored_box(rect.xmin(), rect.xmax()
		, rect.ymin(), rect.ymax(), rect.c);
#endif
	for (int i = start_back - 1; i >= 0; i--) {
		if (P.raw_points[i].y() > rect.ymax()) continue;
		if (P.raw_points[i].y() < rect.ymin()) continue;
		if (P.raw_points[i].c != rect.c) break;
		count++;
		extended = extended.extend_stripe(P.raw_points[i]);
#ifdef GEOWORDLE_PRO_RATIO
		r = extended.ratio();
#ifdef GEOWORDLE_PRO_Extend
		if (extended.getWidth() > extended.getHeight() && r < ratio_lower) {
			deque<colored_box> extension_temp_left;
			add_invalid_candidates(P, ratio_upper, ratio_lower,
				i, start_for, extended, extension_temp_left);
			if (extension_temp_left.size() == 0) break;
			else {
				for (const auto& e : extension_temp_left) {
#ifdef GEOWORDLE_PRO_Font_Size
					if (e.get_text_height() >= min_font_size) {
						candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, count, count));
					}
#else
					candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, count, count));
#endif
				}
			}

		};
#else
		if (rect.getWidth() > rect.getHeight() && r < ratio_lower) break;

#endif
		candidates_set_seed.push_back(tuple<colored_box, size_t, double>(extended, count, count));
#ifdef GEOWORDLE_PRO_Extend
		candidates_set_seed_left.push_back(i);
#endif 
#ifdef GEOWORDLE_PRO_Extend
		if (r >= ratio_lower && r <= ratio_upper)
		{
#ifdef GEOWORDLE_PRO_Font_Size
			if (extended.get_text_height() >= min_font_size) {
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, count, count));
			}
#else
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, count, count));
#endif

		}
		else {
	
			deque<colored_box> extension_temp_left;
			add_invalid_candidates(P, ratio_upper, ratio_lower,
				i, start_for, extended, extension_temp_left);
			for (const auto& e : extension_temp_left) {
#ifdef GEOWORDLE_PRO_Font_Size
				if (e.get_text_height() >= min_font_size) {
					candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, count, count));
				}
#else
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, count, count));
#endif

			}
		}

#else
		if (r >= ratio_lower && r <= ratio_upper)
		{
#ifdef GEOWORDLE_PRO_Font_Size
			if (extended.get_text_height() >= min_font_size) {
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, count, count));
			}
#else
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, count, count));
#endif
		}
#endif
#else
#ifdef GEOWORDLE_PRO_Font_Size
		if (extended.get_text_height() >= min_font_size) {
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, count, count));
		}
#else
		candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, count, count));
#endif


#endif
	}

#ifdef GEOWORDLE_PRO_Font_Size
#ifndef NDEBUG
	debugSolution_fonts_with_auxilaries_temp(P, candidates_set_temp);
#endif
#endif




	//TODO: traverse the current boxes
#ifdef GEOWORDLE_PRO_RATIO
	const auto& v_r = candidates_set_seed;
	size_t left_rectangles_size = candidates_set_seed.size();
#else
	const auto& v_r = candidates_set_temp;
	size_t left_rectangles_size = candidates_set_temp.size();
#endif
	count = 0;
	size_t total_count = 0;
	for (int i = start_for + 1; i < temp_size; i++) {
		if (P.raw_points[i].y() > rect.ymax()) continue;
		if (P.raw_points[i].y() < rect.ymin()) continue;
		if (P.raw_points[i].c != rect.c) break;
		count++;
		for (size_t j = 0; j < left_rectangles_size; j++) {
			total_count = count + get<1>(v_r[j]);
			extended = get<0>(v_r[j]).extend_stripe(P.raw_points[i]);
#ifdef GEOWORDLE_PRO_RATIO
			r = extended.ratio();
#ifdef GEOWORDLE_PRO_Extend
			if (extended.getWidth() > extended.getHeight() && r < ratio_lower) {

				deque<colored_box> extension_temp_right;
				add_invalid_candidates(P, ratio_upper, ratio_lower,
					candidates_set_seed_left[j], i, extended, extension_temp_right);
				if (extension_temp_right.size() == 0) break;
				else {
					for (const auto& e : extension_temp_right) {
#ifdef GEOWORDLE_PRO_Font_Size
						if (e.get_text_height() >= min_font_size) {
							candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, total_count, total_count));
						}
#else
						candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, total_count, total_count));
#endif

					}
				}
			
			}
#else
			if (extended.getWidth() > extended.getHeight() && r < ratio_lower) break;
#endif
#ifdef GEOWORDLE_PRO_Extend
			if (r >= ratio_lower && r <= ratio_upper)
			{
#ifdef GEOWORDLE_PRO_Font_Size
				if (extended.get_text_height() >= min_font_size) {
					candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, total_count, total_count));
				}
#else
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, total_count, total_count));
#endif

			}
			else 
			{
				deque<colored_box> extension_temp_right;
				add_invalid_candidates(P, ratio_upper, ratio_lower,
					candidates_set_seed_left[j], i, extended, extension_temp_right);
				if (extension_temp_right.size() == 0) break;
				else {
					for (const auto& e : extension_temp_right) {
#ifdef GEOWORDLE_PRO_Font_Size
						if (e.get_text_height() >= min_font_size) {
							candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, total_count, total_count));
						}
#else
						candidates_set_temp.push_back(tuple<colored_box, size_t, double>(e, total_count, total_count));
#endif

					}
				}
			}
#else
			if (r >= ratio_lower && r <= ratio_upper)
			{
#ifdef GEOWORDLE_PRO_Font_Size
				if (extended.get_text_height() >= min_font_size) {
					candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, total_count, total_count));
				}
#else
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, total_count, total_count));
#endif

			}
#endif

#else
#ifdef GEOWORDLE_PRO_Font_Size
			if (extended.get_text_height() >= min_font_size) {
				candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, total_count, total_count));
			}
#else
			candidates_set_temp.push_back(tuple<colored_box, size_t, double>(extended, total_count, total_count));
#endif

#endif
		}
	}
#ifdef GEOWORDLE_PRO_Font_Size
#ifndef NDEBUG
	debugSolution_fonts_with_auxilaries_temp(P, candidates_set_temp);
#endif
#endif

}
#endif


void solver::add_penalty(const problem& P) {

};


//!DRAW+++++++++++++++draw+++++++++++++++++++++++++
#ifdef GEOWORDLE_DRAW
void solver::drawSolution(cairo_t* cr, problem& P) {
	double width = P.getWidth();
	double height = P.getHeight();
	for (auto it = solution.begin(); it != solution.end(); ++it) {
#ifdef GEOWORDLE_PRO_Font_Size
		if (font_siz_restriction) {
			if (it->get_text_height()< min_font_size){
				//it->print();
				//continue;
			}
		}
#endif
#ifdef GEOWORDLE_PRO_RATIO
		(*it).draw(cr, width, height, true, 0.3, false);
#else
		(*it).draw(cr, width, height, true, 0.3);
#endif


	}

};
#endif
//!DRAW---------------draw-------------------------


void solver::printSolution(	std::ostream& out)
{

	out << "Solution: " << endl;
	for (auto it = solution.begin(); it != solution.end(); ++it) {
		(*it).print(out);
	}
	out << endl;

}


//xDEBUG+++++++++++++++debug++++++++++++++++++++++++
#ifndef NDEBUG
void solver::debug(problem& P) {
	debugSetting(P);
	debugSolution(P);
};



void solver::debugSolution_fullCover(problem& P) {
#ifdef GEOWORDLE_PRO_Font_Size
	return;
#endif
	bool cover_flag = false;
	for (auto ip = P.raw_points.begin(); ip != P.raw_points.end(); ++ip) {
		//ip->print();
		cover_flag = false;
		for (auto ib = solution.begin(); ib != solution.end(); ++ib) {
#ifndef GEOWORDLE_PRO_Tolerance
			if ((*ib).is_cover(*ip)) {
				cover_flag = true;
				break;
			}
#else 
			if ((*ib).geo_is_cover(*ip)) {
				cover_flag = true;
				break;
			}
#endif


		}
		if (!cover_flag) {
			printSolution();
		
		}
		assert(cover_flag);
	}

#ifdef GEOWORDLE_PRO_Tolerance
	for (auto ib = solution.begin(); ib != solution.end(); ++ib) {
		cover_records record = cover_records(P.number_of_colors());
		for (auto ip = P.raw_points.begin(); ip != P.raw_points.end(); ++ip)
		{

			if ((*ib).geo_is_cover(*ip)) {
				record.increase(ip->c);
			}
		}
		assert(record.ratio() >= ratio_to);

	}
#endif
}


void solver::debugSolution_disjoint() {
	size_t s = solution.size();
	for (unsigned int i = 0; i < s; i++) {
		for (unsigned int j = i + 1; j < s; j++) {
			if (!solution[i].is_disjoint(solution[j])) {
				solution[i].print();
				solution[j].print();
				std::cout << "raw_epsilon_x" << raw_epsilon_x <<std:: endl;
				std::cout << "raw_epsilon_y" << raw_epsilon_y << std::endl;
				assert(false);
			}
		}
	}
};


// test if the solution is valid 

void solver::debugSolution(problem& P) {
	for (auto it = solution.begin(); it != solution.end(); ++it) {
		(*it).debug();
	}

	// full cover
#ifndef GEOWORDLE_PRO_Font_Size
	debugSolution_fullCover(P);
#endif
	//disjoint
	debugSolution_disjoint();
	for (auto it = solution.begin(); it != solution.end(); ++it) {
		(*it).debug();
		assert((*it).c < P.number_of_colors());
	}
	// debug if the pre-computation works
#ifndef GEOWORDLE_PRO_Font_Size 
	assert(P.solution_bound <= solution.size());
	
	vector<size_t> counts(P.number_of_colors(), 0);
	for (const auto& s : solution) {
		counts[s.c]++;
	}
	for (size_t i = 0; i < P.number_of_colors(); i++) {
		assert(counts[i] >= P.solution_bounds[i]);
	}
#endif
};
#ifdef GEOWORDLE_PRO_Font_Size
#ifndef NDEBUG
void solver::debugSolution_fonts_with_auxilaries(problem& P) {
	for (const auto& e : candidates_set) {
		colored_box rect = get<0>(e);
		if (font_siz_restriction) {
			if (floor(rect.get_text_height()) < min_font_size-1) {
				rect.print();
				std::cout << "height" << rect.get_text_height() << std::endl;
				assert(false);
				//continue;
			}
		}

	}
}
#endif
#endif
#ifdef GEOWORDLE_PRO_RATIO
#ifndef NDEBUG
void solver::debugSolution_ratios_with_auxilaries(problem& P) {
	for (const auto& e : candidates_set) {
		colored_box rect = get<0>(e);
		double r = get<2>(P.text_ratios_map[rect.c]);
		//rect.print();
		//std::cout << "test ratio" << r << std::endl;

		rect.debugRatio(r);
		rect.debug();

		assert(rect.c < colorNumber);
		assert(rect.xmax() < P.getWidth());
		assert(rect.ymax() < P.getHeight());
		checkCounts(P, e);
	}
}

void solver::debugRatio(problem& P, deque<colored_box>& candidates_set) {
	for (const auto& e : candidates_set) {
		double r = get<2>(P.text_ratios_map[e.c]);
		//rect.print();
		//std::cout << "test ratio" << r << std::endl;
		e.debugRatio(r);
		assert(e.c < colorNumber);
		assert(e.xmax() < P.getWidth());
		assert(e.ymax() < P.getHeight());

	}
}

void solver::debugRatio(problem& P, deque < tuple <colored_box, size_t, double>> candidates_set) {
	for (const auto& e : candidates_set) {
		auto rect = get<0>(e);
		double r = get<2>(P.text_ratios_map[rect.c]);
		//rect.print();
		//std::cout << "test ratio" << r << std::endl;
		rect.debug();
		assert(rect.c < colorNumber);
		assert(rect.xmax() < P.getWidth());
		assert(rect.ymax() < P.getHeight());
		rect.debugRatio(r);
		for (auto& c : candidates_set) {
			checkCounts(P, c);
		}
	}
}

#endif
#endif

void solver::debugSolution_with_auxilaries(problem& P, bool colored) {
	debugSetting(P);
#ifdef GEOWORDLE_PRO_RATIO
	debugSolution_ratios_with_auxilaries(P);
#endif
#ifndef GEOWORDLE_PRO_Font_Size
	debugSolution_fullCover_with_auxilaries(P);
#endif
	debugSolution_disjoint_with_auxilaries(colored);
};


void solver::checkCounts(problem& P, const tuple <colored_box, size_t, double >& c) {
	size_t count = 0;
	for (auto& p : P.raw_points) {
		if (get<0>(c).geo_is_cover(p)) {
#ifndef GEOWORDLE_PRO_Tolerance			
			assert(p.c == get<0>(c).c);
#endif
#ifdef GEOWORDLE_PRINT
			//p.print();
#endif
			count++;
		}
	}
	assert(count == get<1>(c));
}


#ifndef GEOWORDLE_PRO_Tolerance
void solver::debugMono(problem& P, deque<colored_box>& candidates_set) {
	for (const auto& e : candidates_set) {
		for (const auto& p : P.raw_points) {
			if (e.geo_is_cover(p)) {
				assert(e.c == p.c);
			}
		}

	}
}



#endif

#endif

//xDEBUG---------------debug------------------------

#ifdef GEOWORDLE_PRO_Font_Size
void solver::add_singleton_candidates_font(problem& P, size_t i) {
	size_t color = P.raw_points[i].c;
	double h = min_font_size / 2.0;
	double ratio = get<2>(P.text_ratios_map[color]);
	double w = h / ratio;
// get horizontal 
	w = h / ratio;
	double new_x_min = -1;
	double new_x_max = -1;
	double left_space = P.raw_points[i].x() - raw_extension_epsilon_x;
	double right_space = P.getWidth() - P.raw_points[i].x() - raw_extension_epsilon_x;
	if (left_space + right_space > 2 * w) {
		if (left_space > w && right_space > w) {
			new_x_min = P.raw_points[i].x() - w;
			new_x_max = P.raw_points[i].x() + w;
			singleton_extend_vertical(new_x_min, new_x_max, P, i);
		}
		else {
			bool left_s = left_space < right_space;
			double left_ext = 0;
			double right_ext = 0;
			if (left_s)
			{
				left_ext = left_space;
				right_ext = 2 * w - left_ext;
			}
			else {
				right_ext = right_space;
				left_ext = 2 * w - right_ext;
			}
			new_x_min = P.raw_points[i].x() - left_ext;
			new_x_max = P.raw_points[i].x() + right_ext;
			singleton_extend_vertical(new_x_min, new_x_max, P, i);
		}



		if (right_space > 2 * w) {
			new_x_min = P.raw_points[i].x();
			new_x_max = P.raw_points[i].x() + 2 * w;
			singleton_extend_vertical(new_x_min, new_x_max, P, i);
		}

		if (left_space > 2 * w) {
			new_x_min = P.raw_points[i].x() - 2 * w;
			new_x_max = P.raw_points[i].x() ;
			singleton_extend_vertical(new_x_min, new_x_max, P, i);
		}


	}


		// get vertical 


	double new_y_min = -1;
	double new_y_max = -1;
	double below_space = P.raw_points[i].y() - raw_extension_epsilon_y;
	double above_space = P.getHeight() - P.raw_points[i].y() - raw_extension_epsilon_y;

	if (below_space + above_space > 2 * w) {
		if (above_space > w && below_space > w) {
			new_y_min = P.raw_points[i].y() - w;
			new_y_max = P.raw_points[i].y() + w;
			singleton_extend_horizontal(new_y_min, new_y_max, P, i);
		}
		else {
			bool below_s = below_space < above_space;
			double below_ext = 0;
			double above_ext = 0;
			if (below_s)
			{
				below_ext = below_space;
				above_ext = 2 * w - below_ext;
			}
			else {
				above_ext = above_space;
				below_ext = 2 * w - above_ext;
			}
			new_y_min = P.raw_points[i].y() - below_ext;
			new_y_max = P.raw_points[i].y() + above_ext;
			singleton_extend_horizontal(new_y_min, new_y_max, P, i);
		}
		if (above_space > 2 * w) {
			new_y_min = P.raw_points[i].y();
			new_y_max = P.raw_points[i].y() + 2 * w;
			singleton_extend_horizontal(new_y_min, new_y_max, P, i);
		}

		if (below_space > 2 * w) {
			new_y_min = P.raw_points[i].y() - 2 * w;
			new_y_max = P.raw_points[i].y();
			singleton_extend_horizontal(new_y_min, new_y_max, P, i);
		}
	}

}



void solver::singleton_extend_vertical(double new_x_min, double new_x_max, problem& P, size_t i) {
	colored_box rect = colored_box(new_x_min, new_x_max, P.raw_points[i].y(), P.raw_points[i].y(), P.raw_points[i].c);
	assert(rect.geo_is_cover(P.raw_points[i]));
	// the indices define the points in the vertical stripe of the box
	colored_point min_object = colored_point(rect.xmin());
	int l_index = lower_bound(P.raw_points.begin(), P.raw_points.end(), min_object, sortbyX) - P.raw_points.begin();
	assert(l_index >= 0);
	assert(l_index < P.number_of_points());
	colored_point max_object = colored_point(rect.xmax());
	int u_index = upper_bound(P.raw_points.begin(), P.raw_points.end(), max_object, sortbyX) - P.raw_points.begin();
	assert(u_index > 0);
	assert(u_index <= P.number_of_points());
	deque<colored_box> candidates_set_temp_h;
	extend_vertical(P, min_font_size, rect, l_index, u_index, candidates_set_temp_h);
	for (const auto& e : candidates_set_temp_h) {
		candidates_set.push_back(tuple<colored_box, double, double>(e, 1, 1.2));
#ifdef GEOWORDLE_PRO_RATIO
#ifndef NDEBUG
		debugSolution_ratios_with_auxilaries(P);
#endif
#endif

	}




};

void solver::singleton_extend_horizontal(double new_y_min, double new_y_max, problem& P, size_t i) {
	deque<colored_box> candidates_set_temp_v;
	colored_box rect = colored_box(
		P.raw_points[i].x()
		, P.raw_points[i].x(),
		new_y_min
		, new_y_max,
		P.raw_points[i].c);
	assert(rect.geo_is_cover(P.raw_points[i]));
	// the indices define the points in the vertical stripe of the box
	extend_horizontal(P, min_font_size, rect, i, i, candidates_set_temp_v);
	for (const auto& e : candidates_set_temp_v) {
		candidates_set.push_back(tuple<colored_box, double, double>(e, 1, 1.1));
	}
#ifdef GEOWORDLE_PRO_RATIO
#ifndef NDEBUG
	debugSolution_ratios_with_auxilaries(P);
#endif
#endif


#ifdef GEOWORDLE_PRO_Font_Size
#ifndef NDEBUG
	debugSolution_fonts_with_auxilaries(P);
#endif
#endif
};
#endif
// 1.step: decide extension direction
//*      < r    >r
//* w>h   v     h 
//* w<h   h     v


#ifdef GEOWORDLE_PRO_RATIO


// 

void solver::add_invalid_candidates(problem& P, double ratio_upper,
double ratio_lower, size_t index_left, size_t index_right,
	colored_box box, deque<colored_box>& candidates_set_temp)
{

	assert(index_left < index_right);

	double w = box.getWidth();
	double h = box.getHeight();
	double r = box.ratio();
	double tight_ratio = get<2>(P.text_ratios_map[box.c]);

	assert(r > ratio_upper || r < ratio_lower);
	bool extend_hor = true;
	if (w > h)
	{
		if( r < ratio_lower)
		{
#ifdef GEOWORDLE_PRO_Font_Size
			if (font_siz_restriction && w* ratio_lower < min_font_size)
			{
				// 3*3 options
				extend_vertical_font(P, tight_ratio, box, index_left, index_right, candidates_set_temp);
			}
#else
			extend_vertical(P, w * ratio_lower, box, index_left, index_right, candidates_set_temp);
#endif
#ifndef NDEBUG
			debugRatio(P, candidates_set_temp);
#endif
		}
		// horizontal square
		else
		{
			assert(r > ratio_upper);

#ifdef GEOWORDLE_PRO_Font_Size
			if (font_siz_restriction && h < min_font_size)
			{	
				// 3*3 options
				extend_vertical_font(P, tight_ratio, box, index_left, index_right, candidates_set_temp);
#ifndef NDEBUG
				debugRatio(P, candidates_set_temp);
#endif
			}
#else
		extend_horizontal(P, h/ ratio_upper, box, index_left, index_right, candidates_set_temp);
#endif

#ifndef NDEBUG
			debugRatio(P, candidates_set_temp);
#endif
		}

	}
	else
	{
		// vertical square
		if (r > ratio_upper)
		{

#ifdef GEOWORDLE_PRO_Font_Size
			if (font_siz_restriction && w < min_font_size)
			{	// 3*3 options
				extend_horizontal_font(P, tight_ratio, box, index_left, index_right, candidates_set_temp);
#ifndef NDEBUG
				debugRatio(P, candidates_set_temp);
#endif
			}
#else
			extend_vertical(P, w/ ratio_upper, box, index_left, index_right, candidates_set_temp);
#ifndef NDEBUG
			debugRatio(P, candidates_set_temp);
#endif
#endif
		}
		else 
		{
			assert(r < ratio_lower);
#ifdef GEOWORDLE_PRO_Font_Size
			if (font_siz_restriction && h * ratio_lower < min_font_size)
			{	// 3*3 options
				extend_horizontal_font(P, tight_ratio, box, index_left, index_right, candidates_set_temp);
			}
#else
			extend_horizontal(P, h*ratio_lower, box, index_left, index_right, candidates_set_temp);
#endif
#ifndef NDEBUG
			debugRatio(P, candidates_set_temp);
#endif
		}
	}
#ifndef NDEBUG
	debugRatio(P, candidates_set_temp);
#endif
};





#endif
#if defined GEOWORDLE_PRO_RATIO
#if defined GEOWORDLE_PRO_Font_Size
void solver::extend_vertical_font(problem& P, double tight_ratio,
	colored_box box, size_t index_left, size_t index_right,
	deque<colored_box>& candidates_set_temp)
{
	assert(box.getHeight() < min_font_size);
	// vertical extension to minimum font size
	deque<colored_box> candidates_set_temp_v;
	extend_vertical(P, min_font_size, box, index_left, index_right, candidates_set_temp_v);
	double w = min_font_size / tight_ratio;
	for (const auto& e : candidates_set_temp_v) {
		/*
		if (e.get_text_height() < (min_font_size - 1)) {
			std::cout << "the box info" << std::endl;
			e.print();
			std::cout << "min_font_size"<< min_font_size << std::endl;
		}
		*/
		assert(e.getHeight() >= (min_font_size - 1));
		assert(e.xmax() == box.xmax());
		assert(e.xmin() == box.xmin());
		if(box.getWidth() < w){
			extend_horizontal(P, w, e, index_left, index_right, candidates_set_temp);
		}
	}
	assert(candidates_set_temp.size() <= 9);
}
// TODO: add the font_size extension
void solver::extend_horizontal_font(problem& P, double tight_ratio,
	colored_box box, size_t index_left, size_t index_right,
	deque<colored_box>& candidates_set_temp)
{
	assert(box.getWidth() < min_font_size);
	// vertical extension to minimum font size
	deque<colored_box> candidates_set_temp_v;
	extend_horizontal(P, min_font_size, box, index_left, index_right, candidates_set_temp_v);
	double w = min_font_size / tight_ratio;
	for (const auto& e : candidates_set_temp_v) {
		if (e.getWidth() < (min_font_size - 1)) {
			std::cout << "the box info" << std::endl;
			e.print();
			std::cout << "min_font_size" << min_font_size << std::endl;
		}
		assert(e.getWidth() >= (min_font_size-1));
		assert(e.ymax() == box.ymax());
		assert(e.ymin() == box.ymin());
		if(w > box.getHeight()) extend_vertical(P, w, e, index_left, index_right, candidates_set_temp);
	}
	assert(candidates_set_temp.size() <= 9);
}

#endif
#endif


#ifndef NDEBUG
#ifdef GEOWORDLE_PRO_RATIO
void solver::debug_point_distance(problem& P, deque<colored_box>& candidates_set) {
	for (const auto& e : candidates_set) {
		double r = get<2>(P.text_ratios_map[e.c]);
		e.debugRatio(r);

	}
}
#endif
#endif



#if defined GEOWORDLE_PRO_RATIO || defined GEOWORDLE_PRO_Font_Size
void solver::extend_vertical(problem& P, double h,
	colored_box box, size_t index_left, size_t index_right,
	deque<colored_box>& candidates_set_temp)
{

	assert(box.getHeight() == 0 || P.raw_points[index_left].x() >= box.xmin());
	assert(box.getHeight() == 0 || P.raw_points[index_right].x() <= box.xmax());
	assert(box.getHeight() == 0 || P.raw_points[index_left].y() >= box.ymin());
	assert(box.getHeight() == 0 || P.raw_points[index_right].y() <= box.ymax());
	assert(h > box.getHeight());
#ifndef GEOWORDLE_PRO_Tolerance
	assert(box.getHeight() == 0 || P.raw_points[index_left].c == P.raw_points[index_right].c);
#endif


	// find the vertical bounds 
	double above_bound = P.getHeight();
	double below_bound = 0;

	for (size_t i = index_left; i < index_right; i++) {
		//std::cout << "peaks: ";
		//P.raw_points[i].print();
		if (P.raw_points[i].y() > box.ymax() && P.raw_points[i].y() < above_bound)
		{
			above_bound = P.raw_points[i].y();
		}
		if (P.raw_points[i].y() < box.ymin() && P.raw_points[i].y() > below_bound)
		{
			below_bound = P.raw_points[i].y();
		};
	}

	above_bound = above_bound - (P.box_y_epsilon + raw_extension_epsilon_y);
	below_bound += P.box_y_epsilon + raw_extension_epsilon_y;

	assert(below_bound < box.ymin());
	assert(above_bound > box.ymax());

	//double w = box.getWidth();
	//if (box.getHeight() < w)  h = w * ratio_upper;
	//else h = w / ratio_upper;

	double h_diff = h - box.getHeight();
	assert(h_diff > 0);
	double below_space = box.ymin() - below_bound;
	double above_space = above_bound - box.ymax();
	assert(below_space > 0);
	assert(above_space > 0);
	//1.can: using left space as possible
	if (h_diff < below_space)
	{
		colored_box below_extended = box.extend_stripe(h_diff, 2);
		candidates_set_temp.push_back(below_extended);
#ifndef NDEBUG
		//debugRatio(P, candidates_set_temp);
#ifndef GEOWORDLE_PRO_Tolerance
		//debugMono(P, candidates_set_temp);
#endif
#endif
	}

	//2.can: right
	if (h_diff < above_space)
	{
		colored_box above_extended = box.extend_stripe(h_diff, 0);
		candidates_set_temp.push_back(above_extended);
#ifndef NDEBUG
		//debugRatio(P, candidates_set_temp);
#ifndef GEOWORDLE_PRO_Tolerance
		debugMono(P, candidates_set_temp);
#endif
#endif
	}

	//3.can:balance if possible 
	// otherwise, firstly extend  the smaller side greedily
	if (h_diff < above_space + below_space) {
		double h_w_diff = h_diff / 2.0;
		if (below_space > h_w_diff && above_space > h_w_diff) {
			colored_box both_extended = box.extend_stripe(0, 0, h_w_diff, h_w_diff);
			candidates_set_temp.push_back(both_extended);
#ifndef NDEBUG
			//debugRatio(P, candidates_set_temp);
#ifndef GEOWORDLE_PRO_Tolerance
			//debugMono(P, candidates_set_temp);
#endif
#endif
		}
		else {
			bool below_s = below_space < above_space;
			double below_ext = 0;
			double above_ext = 0;
			if (below_s)
			{
				below_ext = below_space;
				above_ext = h_diff - below_ext;
			}
			else {
				above_ext = above_space;
				below_ext = h_diff - above_ext;
			}

			colored_box both_extended = box.extend_stripe(0, 0, below_ext, above_ext);
			candidates_set_temp.push_back(both_extended);
#ifndef NDEBUG
			//debugRatio(P, candidates_set_temp);
#ifndef GEOWORDLE_PRO_Tolerance
			debugMono(P, candidates_set_temp);
#endif
#endif
		}
	}

	assert(candidates_set_temp.size() <= 9);
};



void solver::extend_horizontal(problem& P, double w,
	colored_box box, size_t index_left, size_t index_right,
	deque<colored_box>& candidates_set_temp)
{
	double left_bound = raw_extension_epsilon_x;
	double right_bound = P.getWidth() - raw_extension_epsilon_x;
	assert(box.getWidth() == 0 || P.raw_points[index_left].x() >= box.xmin());
	assert(box.getWidth() == 0 || P.raw_points[index_right].x() <= box.xmax());
	assert(box.getWidth() == 0 || P.raw_points[index_left].y() >= box.ymin());
	assert(box.getWidth() == 0 || P.raw_points[index_right].y() <= box.ymax());
	assert(w > box.getWidth());
#ifndef GEOWORDLE_PRO_Tolerance
	assert(P.raw_points[index_left].c == P.raw_points[index_right].c);
#endif
	// find rightmost left
	if (index_left != 0) {
		assert(index_left - 1 >= 0);
		left_bound = P.raw_points[index_left - 1].x() + P.box_x_epsilon + raw_extension_epsilon_x;
		assert(box.xmin() - left_bound > 0);
	}
	// find leftmost right
	if (index_right != P.number_of_points() - 1) {
		assert(index_right + 1 < P.number_of_points());
		right_bound = P.raw_points[index_right + 1].x() - P.box_x_epsilon - raw_extension_epsilon_x;
	}

	//double h = box.getHeight();
	//double w = 0;
	//if(box.getWidth() > h)  w = h / ratio_upper;
	//else w = h * ratio_upper;

	double w_diff = w - box.getWidth();
	double left_space = box.xmin() - left_bound;
	double right_space = right_bound - box.xmax();
	//1.can: using left space as possible
	if (w_diff < left_space)
	{
		colored_box left_extended = box.extend_stripe(w_diff, 1);

		assert(left_extended.xmin() <= box.xmin());
		assert(left_extended.xmax() >= box.xmax());
		candidates_set_temp.push_back(left_extended);
	}
	//2.can: right
	if (w_diff < right_space)
	{
		colored_box right_extended = box.extend_stripe(w_diff, 3);
		assert(right_extended.xmin() <= box.xmin());
		assert(right_extended.xmax() >= box.xmax());
		candidates_set_temp.push_back(right_extended);
	}
	//3.can:balance if possible 
	// otherwise, firstly extend  the smaller side greedily
	if (w_diff < left_space + right_space) {
		double h_w_diff = w_diff / 2.0;
		if (left_space > h_w_diff && right_space > h_w_diff) {
			colored_box both_extended = box.extend_stripe(h_w_diff, h_w_diff, 0, 0);

			assert(both_extended.xmin() <= box.xmin());
			assert(both_extended.xmax() >= box.xmax());
			candidates_set_temp.push_back(both_extended);
		}
		else {
			bool left_s = left_space < right_space;
			double left_ext = 0;
			double right_ext = 0;
			if (left_s)
			{
				left_ext = left_space;
				right_ext = w_diff - left_ext;
			}
			else {
				right_ext = right_space;
				left_ext = w_diff - right_ext;
			}

			colored_box both_extended = box.extend_stripe(left_ext, right_ext, 0, 0);

			assert(both_extended.xmin() <= box.xmin());
			assert(both_extended.xmax() >= box.xmax());
			candidates_set_temp.push_back(both_extended);
		}
	}
#ifdef GEOWORDLE_PRO_Font_Size
#ifndef NDEBUG
	debugSolution_fonts_with_auxilaries(P);
#endif
#endif
#ifndef NDEBUG
	//debugRatio(P, candidates_set_temp);
#ifndef GEOWORDLE_PRO_Tolerance
	debugMono(P, candidates_set_temp);
#endif
#endif
	assert(candidates_set_temp.size() <= 9);
};


#endif