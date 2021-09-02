#pragma once
#include "problem.h"
#include <deque>
class solver{
public:
	solver() {};
	virtual ~solver() {};
	virtual void set(problem& P, size_t boxN= 0, size_t N = 3, int gurobi_focus = 2, string solver_id = "sat",
		const std::vector<size_t>* boxNumbers = NULL, bool M_flag_p = true)
	{
		colorNumber = P.number_of_colors();
		pointNumber = P.number_of_points();
		//std::sort(P.points.begin(), P.points.end());
	
	};
	inline int candidates_set_size() {
		return candidates_set.size();
	}
	virtual SolveStatus solve(problem& P, jsonM& measures, size_t N = 3 ) = 0;

	//+++++++++++++++test+++++++++++++++++++++++++
	SolveStatus test(problem& P, jsonM& measures);
	//---------------test-------------------------
	template <typename T>
	friend class recursive_full_workflow;
	template <typename T>
	friend class recursive_colored_full_workflow;
	// solution container
	size_t pointNumber;
	size_t colorNumber;
	vector<colored_box> solution;


	void printSolution(std::ostream& out = std::cout);



protected:

	deque < tuple <colored_box, size_t, double>> candidates_set;
	virtual void set_candidate_set(problem& P, jsonM& measures);
	virtual void get_points_stripe(size_t start_back,
		size_t start_for, problem& P, deque<tuple<colored_box, size_t, double>>& candidates_set_temp);
	virtual void add_penalty(const problem& P);
	virtual void setSolution(problem& P) = 0;
//!DRAW+++++++++++++++draw+++++++++++++++++++++++++
#ifdef GEOWORDLE_DRAW
	virtual void drawSolution(cairo_t* cr, problem& P);
#endif
//!DRAW---------------draw-------------------------


//?PRINT+++++++++++++++PRINT+++++++++++++++++++++++++
#ifdef GEOWORDLE_PRINT
	//+++++++++++++++print+++++++++++++++++++++++++
	virtual void printSolution_with_auxilaries() = 0;
	//---------------print-------------------------
#endif
//?PRINT---------------PRINT-------------------------

//xDEBUG+++++++++++++++debug++++++++++++++++++++++++
#ifndef NDEBUG
	// test if the instance is valid
	virtual void debugSetting(problem& P) = 0;
	virtual void checkCounts(problem& P, const tuple <colored_box, size_t, double >& c);
	// possible properties
	virtual void debugSolution_fullCover_with_auxilaries(problem& P) = 0;
	virtual void debugSolution_disjoint_with_auxilaries(bool colored = false) = 0;

	// test if the solution is valid
	void debugSolution(problem& P);
	void debugSolution_fullCover(problem& P);
	void debugSolution_disjoint();
	void debug(problem& P);
	virtual void debugSolution_with_auxilaries(problem& P, bool colored = false);
	void debugSolution_ratios_with_auxilaries(problem& P);


#if defined GEOWORDLE_PRO_RATIO 
	void debug_point_distance(problem& P, deque<colored_box>& candidates_set);
	void debugRatio(problem& P, deque<colored_box>& candidates_set);
	void debugRatio(problem& P, deque < tuple <colored_box, size_t, double>> candidates_set);
#endif
#ifndef GEOWORDLE_PRO_Tolerance
	void debugMono(problem& P, deque<colored_box>& candidates_set);
#endif
#endif
	inline void debugSolution_basic_with_auxilaries(problem& P) {
		for (const auto& e : candidates_set) {
			colored_box rect = get<0>(e);
#ifdef GEOWORDLE_PRINT
			//rect.print();
#endif
#ifndef NDEBUG
			rect.debug();
#endif
		}
	
	};
//xDEBUG---------------debug------------------------
private:
#if defined GEOWORDLE_PRO_RATIO || defined GEOWORDLE_PRO_Font_Size

	void extend_horizontal(problem& P, double w,
		colored_box box, size_t index_left, size_t index_right,
		deque<colored_box>& candidates_set_temp);
	void extend_vertical(problem& P, double h,
		colored_box box, size_t index_left, size_t index_right,
		deque<colored_box>& candidates_set_temp);
#endif
#if defined GEOWORDLE_PRO_RATIO
	void add_invalid_candidates(problem& P, double ratio_upper,
		double ratio_lower, size_t index_left, size_t index_right,
		colored_box box, deque<colored_box>& candidates_set_temp);
#if defined GEOWORDLE_PRO_Font_Size
	void extend_horizontal_font(problem& P, double ratio_upper,
		colored_box box, size_t index_left, size_t index_right,
		deque<colored_box>& candidates_set_temp);
	
	
	void extend_vertical_font(problem& P, double ratio_upper,
		colored_box box, size_t index_left, size_t index_right,
		deque<colored_box>& candidates_set_temp);


#endif
#endif


#ifdef GEOWORDLE_PRO_Font_Size
	void add_singleton_candidates_font(problem& P, size_t index);
	void singleton_extend_vertical(double new_x_min, double new_x_max, problem& P, size_t i);
	void singleton_extend_horizontal(double new_y_min, double new_y_max, problem& P, size_t i);
#ifndef NDEBUG
	void debugSolution_fonts_with_auxilaries(problem& P);
	inline void debugSolution_fonts_with_auxilaries_temp(problem& P, deque<tuple<colored_box, size_t, double>>& candidates_set_temp) {
		for (const auto e : candidates_set_temp) {
			assert(get<0>(e).get_text_height() >= min_font_size);
		}
	};
	
	inline void debugSolution_fonts_with_auxilaries_temp2(problem& P, deque<colored_box>& candidates_set_temp) {
		for (const auto e : candidates_set_temp) {
			assert(e.get_text_height() >= min_font_size);
		}
	};
#endif

#endif
};


#ifdef GEOWORDLE_PRO_Tolerance
struct cover_records {
	vector<size_t> counts;
	int domi_color = -1;
	size_t sum = 0;

	cover_records(size_t color_n) {
		counts = vector<size_t>(color_n, 0);
	}
	void increase(size_t col) {
		counts[col]++;
		sum++;
		if (domi_color  < 0 || counts[col] > counts[domi_color])
		{
			domi_color = col;
		}

	}

	inline double  ratio() {
		assert(counts[domi_color] / (double)sum > 0);
		return counts[domi_color] /(double)sum;
	}
	inline size_t miss_cover_number() {
		assert(sum >= counts[domi_color]);
		return sum - counts[domi_color];
	}
	cover_records merge(const cover_records& other) {
		size_t temp = counts.size();
		assert(temp == other.counts.size());
		cover_records merged = cover_records(temp);

		for (int i = 0; i < temp; i++) {
			merged.counts[i] = counts[i] + other.counts[i];
		}
		for (int i = 0; i < temp; i++) {
			if (merged.domi_color  < 0 || merged.counts[i] > merged.counts[merged.domi_color])
			{
				merged.domi_color = i;
			}
		}
		merged.sum = sum + other.sum;
		return merged;
	
	}
};


#endif

