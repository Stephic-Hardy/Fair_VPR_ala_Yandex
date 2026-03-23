#include <gtest/gtest.h>
#include <fstream>

#include "../utils/json_parser.hpp"
#include "../src/first_step.hpp"

using namespace JsonParser;

TEST(TestFirstStep, TestProblem1) {
    InputData data;
    ASSERT_TRUE(JsonParser::ParseInputDataFromJson(std::string(DATA_DIR) + "/first_step_problem_1.json", data));
    auto answer = DoFirstStep(data);
    EXPECT_EQ(answer.value, 500);
    EXPECT_EQ(answer.vertexes.size(), 7);
    EXPECT_EQ(answer.vertexes[0], 0);
    EXPECT_EQ(answer.vertexes[answer.vertexes.size() - 1], 0);
    auto vertexes_size = answer.vertexes.size() - 2; // убираем старт и финиш
    ASSERT_TRUE(vertexes_size <= data.max_load && vertexes_size >= data.min_load);
}

TEST(TestFirstStep, TestProblem2) {
    InputData data;
    ASSERT_TRUE(JsonParser::ParseInputDataFromJson(std::string(DATA_DIR) + "/first_step_problem_2.json", data));
    auto answer = DoFirstStep(data);
    EXPECT_EQ(answer.value, -1900);
    EXPECT_EQ(answer.vertexes.size(), 4);
    EXPECT_EQ(answer.vertexes[0], 0);
    EXPECT_EQ(answer.vertexes[answer.vertexes.size() - 1], 0);
    auto vertexes_size = answer.vertexes.size() - 2; // убираем старт и финиш
    ASSERT_TRUE(vertexes_size <= data.max_load && vertexes_size >= data.min_load);
}




