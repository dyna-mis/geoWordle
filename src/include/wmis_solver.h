#pragma once
#include "solver.h"
#include "graph.h"
// IS: maximum weighted independent set solver
class wmis_solver :public solver {
public:
	// output the dimacs file
	void set(problem& P, size_t boxN, size_t N = 3, int gurobi_focus = 2, string solver_id = "sat",
		const std::vector<size_t>* boxNumbers = NULL, bool M_flag_p = true) override;
	SolveStatus solve(problem& P, jsonM& measures, size_t N = 3) override;

protected:
	void setSolution(problem& P) override;
	//?PRINT+++++++++++++++PRINT+++++++++++++++++++++++++
#ifdef GEOWORDLE_PRINT
	//+++++++++++++++print+++++++++++++++++++++++++
	void printSolution_with_auxilaries() override;
	//---------------print-------------------------
#endif
//?PRINT---------------PRINT-------------------------

//xDEBUG+++++++++++++++debug++++++++++++++++++++++++
#ifndef NDEBUG
	// test if the instance is valid
	void debugSetting(problem& P) override;

	// possible properties
	void debugSolution_fullCover_with_auxilaries(problem& P) override;
	void debugSolution_disjoint_with_auxilaries(bool colored = false) override;
#endif
//xDEBUG---------------debug------------------------
private:
	void counting_conflists(jsonM& measures);
	Graph cg;
	//default 3: maxhs
	//0: kamis_locla
	//otherwise: kamis_reduce
	size_t solver_index;
	string file_name;
	vector<size_t> solution_indices;
	int set_conflict_graph();
	void write_solver_input_maxHS();
	void write_solver_input_kamis();
	void getMAXHS_maxHS();
	void readWCNF_maxHS();
	void getMAXHS_kamis();
	void readWCNF_kamis();

	inline string getAppendix() {
		string appendix;
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
		return appendix;
	}

};
