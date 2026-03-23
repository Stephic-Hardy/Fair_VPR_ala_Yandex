#include "multi_agent_solver.hpp"
#include "single_agent_solver.hpp"

#include <iostream>

std::vector<Solution> SolveMultiAgent(const InputData &input, uint64_t time_limit){
    std::vector<Solution> all_routes;

    InputData current_input = input;

    uint64_t points_left = input.points_count - 1;

    while (points_left > 0){
        Solution sol = SolveSingleAgent(current_input, time_limit);

        if (sol.route.size() <= 2){
            break;
        }

        all_routes.push_back(sol);

        for (uint64_t p : sol.route) {
            if (p == 0) continue;


            int64_t unreachable_dist = current_input.max_distance * 2 + 1;

            for (size_t i = 0; i < current_input.points_count; ++i){
                current_input.distance_matrix[i][p] = unreachable_dist;
                current_input.distance_matrix[p][i] = unreachable_dist;

            }
            points_left--;
            std::cout << points_left << std::endl;
        }
    }

    return all_routes;
}