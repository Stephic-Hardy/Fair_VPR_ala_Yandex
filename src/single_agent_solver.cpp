#include "single_agent_solver.hpp"
#include "src/first_step.hpp"
#include "src/branch_and_bound.hpp"

Solution BuildSolution(const InputData& input,  const std::vector<uint64_t>& vertexes) {
    Solution sol;
    sol.route = vertexes;
    sol.solution_size = static_cast<uint64_t>(vertexes.size());

    if (sol.solution_size <= 2) {
        sol.total_time = 0;
        sol.total_distance = 0;
        sol.total_value = 0;
        return sol;
    }
    sol.solution_size -= 2;

    uint64_t total_time = 0;
    uint64_t total_distance = 0;
    int64_t total_value = 0;

    for (size_t i = 0; i + 1 < vertexes.size(); ++i) {
        uint64_t from = vertexes[i];
        uint64_t to = vertexes[i + 1];

        if (from < input.distance_matrix.size() && to < input.distance_matrix[from].size()) {
            total_distance += input.distance_matrix[from][to];
        }

        if (!input.time_matrix.empty()) {
            size_t layer = std::min<size_t>(0, input.time_matrix.size() - 1);
            if (from < input.time_matrix[layer].size() && to < input.time_matrix[layer][from].size()) {
                total_time += input.time_matrix[layer][from][to];
            }
        }

        if (to > 0 && (to - 1) < input.point_service_times.size()) {
            total_time += input.point_service_times[to - 1];
        }

        if (to > 0 && (to - 1) < input.point_scores.size()) {
            total_value += input.point_scores[to - 1];
        }
    }

    sol.total_time = total_time;
    sol.total_distance = total_distance;
    sol.total_value = total_value;

    return sol;
}


Solution SolveSingleAgent(const InputData &input, uint64_t time_limit){
    auto firstStepAnswer = DoFirstStep(input).vertexes;

    if (firstStepAnswer.size() <= 2){
        return BuildSolution(input, firstStepAnswer);
    }

    size_t k = firstStepAnswer.size();
    std::vector<std::vector<int64_t>> sub_matrix(k, std::vector<int64_t>(k,0));
    for (size_t i = 0; i < k; ++i){
        for (size_t j = 0; j < k; ++j){
            sub_matrix[i][j] = input.distance_matrix[firstStepAnswer[i]][firstStepAnswer[j]];
        }
    }

    auto branchAndBound = BranchAndBound(time_limit);

    std::vector<int> local_in(k);
    for (int i = 0; i < k; ++i){
        local_in[i] = i;
    }

    auto local_out = branchAndBound.findOptimalRoute(sub_matrix, local_in);

    std::vector<uint64_t> global_out;

    global_out.reserve(local_out.size());
    for (int idx : local_out){
        global_out.push_back(firstStepAnswer[idx]);
    }
    return BuildSolution(input, global_out);
}

