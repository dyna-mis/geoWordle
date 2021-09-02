/*
 * problem_CELL.h
 *
 *  Created on: Aug 6, 2020
 *      Author: guangping
 */

#pragma once

#include <iostream>
#include "problem.h"
#include "solver.h"

class greedy_solver:public solver{
public:
	greedy_solver() {
	};
	virtual ~greedy_solver() {};
	void set(problem& P, size_t boxN=0, size_t N = 3,  int gurobi_focus = 1, string solver_id = "sat",
		const std::vector<size_t>* boxNumbers = NULL, bool M_flag_p = true) override;


	SolveStatus solve(problem& P, jsonM& measures,  size_t N = 3) override;

protected:
	vector<colored_box> greedy_solution;
	void setSolution(problem& P) override;
	virtual size_t count_covered(size_t index);


//!DEBUG+++++++++++++++debug++++++++++++++++++++++++
#ifndef NDEBUG
	virtual void checkOrdering();
	void debugSetting(problem& P) override;
	void debugSolution_fullCover_with_auxilaries(problem& P) override;
	void debugSolution_disjoint_with_auxilaries(bool colored = false) override;
#endif
//!DEBUG---------------debug------------------------


//?PRINT+++++++++++++++PRINT+++++++++++++++++++++++++
#ifdef GEOWORDLE_PRINT
	//+++++++++++++++print+++++++++++++++++++++++++
	void printSolution_with_auxilaries() override;
	//---------------print-------------------------
#endif
//?PRINT---------------PRINT-------------------------


};


