#include "greedy_solver_area.h"
void greedy_solver_area::add_penalty(const problem& P) {
	double minimum_area = dop_raw_epsilon* dop_raw_epsilon;
	for (auto& e : candidates_set) {
		get<2>(e) = get<1>(e) + minimum_area/get<0>(e).area();
	}
};
