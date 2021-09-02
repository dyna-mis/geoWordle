#include "greedy_solver_ratio.h"
void greedy_solver_ratio::add_penalty(const problem& P) {
	for (auto& e : candidates_set) {
		double ratio = 1.0 / strlen(P.texts[get<0>(e).c]);
		get<2>(e) = get<1>(e) + abs(get<0>(e).ratio()- ratio);
	}
};
