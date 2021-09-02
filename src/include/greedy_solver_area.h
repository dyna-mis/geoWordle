#pragma once
#include "greedy_solver.h"
class greedy_solver_area :public greedy_solver{
protected:
	void add_penalty(const problem& P) override;
};
