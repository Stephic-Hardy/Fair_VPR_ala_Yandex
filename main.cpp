#include "utils/json_parser.hpp"
#include "src/multi_agent_solver.hpp"
#include "src/single_agent_solver.hpp"
#include "src/first_step.hpp"
#include "src/branch_and_bound.hpp"


#ifdef DEBUG
#define LOG_LINE std::cout << __FILE__ << ":" << __LINE__ << std::endl;
#endif

int main(int argc, char *argv[]) {
    ProgramArguments args;
    if (!ParseProgramArguments(argc, argv, args)) {
        return -1;
    }

    InputData input;
    if (!JsonParser::ParseInputDataFromJson(args.problemJsonPath, input)) {
        return -2;
    }
    auto multi_solution = SolveMultiAgent(input, args.time);

    if (!JsonParser::WriteSolutionToJsonFile(args.solutionJsonPath, multi_solution)){
        return -3;
    }

    return 0;
}