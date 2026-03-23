#include "utils/json_parser.hpp"
#include "src/first_step.hpp"
#include "src/branch_and_bound.hpp"


#ifdef DEBUG
#define LOG_LINE std::cout << __FILE__ << ":" << __LINE__ << std::endl;
#endif

// собирает решение по входным данным и пути
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
    // в ответе не учитывается посещение базы
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

Solution Solve(const InputData &input, uint64_t time_limit) {

    auto firstStepAnswer = DoFirstStep(input).vertexes;
    auto branchAndBound = BranchAndBound(time_limit);
    // временное решение, надо будет договориться в каком формате храним веришны
    std::vector<uint64_t> secondStepAnswer = [&]{
        std::vector<int> tmp_in;
        tmp_in.reserve(firstStepAnswer.size());
        std::transform(firstStepAnswer.begin(), firstStepAnswer.end(), std::back_inserter(tmp_in),
                    [](uint64_t x){ return static_cast<int>(x); });

        auto tmp_out = branchAndBound.findOptimalRoute(input.distance_matrix, tmp_in);

        std::vector<uint64_t> out;
        out.reserve(tmp_out.size());
        std::transform(tmp_out.begin(), tmp_out.end(), std::back_inserter(out),
                    [](int x){ return static_cast<uint64_t>(x); });

        return out;
    }();
    return BuildSolution(input, secondStepAnswer);
}

int main(int argc, char *argv[]) {
    ProgramArguments args;
    if (!ParseProgramArguments(argc, argv, args)) {
        return -1;
    }

    InputData input;
    if (!JsonParser::ParseInputDataFromJson(args.problemJsonPath, input)) {
        return -2;
    }
    // max_time используется для времени в пути, а не для времени работы шага два
    if (!JsonParser::WriteSolutionToJsonFile(args.solutionJsonPath, Solve(input, args.time))){
        return -3;
    }

    return 0;
}