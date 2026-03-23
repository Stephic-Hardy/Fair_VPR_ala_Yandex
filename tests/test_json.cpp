#include <gtest/gtest.h>
#include <fstream>
#include "../utils/json_parser.hpp"

using namespace JsonParser;

TEST(JsonParserTest, ParseInputDataFromJson) {
    InputData data;
    ASSERT_TRUE(JsonParser::ParseInputDataFromJson(std::string(DATA_DIR) + "/problem.json", data));

    EXPECT_EQ(data.points_count, 3);
    EXPECT_EQ(data.min_load, 1);
    EXPECT_EQ(data.max_load, 5);
    EXPECT_EQ(data.max_time, 100);
    EXPECT_EQ(data.max_distance, 50);
    EXPECT_EQ(data.distance_matrix[0][1], 10);
    EXPECT_EQ(data.time_matrix[0][2][1], 1);
    EXPECT_EQ(data.point_scores[1], 20);
    EXPECT_EQ(data.point_service_times[0], 5);
}

TEST(JsonParserTest, ParseSolutionFromJson) {
    Solution solution;
    ASSERT_TRUE(JsonParser::ParseSolutionFromJson(std::string(DATA_DIR) + "/solution.json", solution));

    EXPECT_EQ(solution.route.size(), 4);
    EXPECT_EQ(solution.solution_size, 2);
    EXPECT_EQ(solution.total_time, 15);
    EXPECT_EQ(solution.total_distance, 45);
    EXPECT_EQ(solution.total_value, 30);
}

TEST(JsonParserTest, WriteSolutionToJsonFile) {
    Solution solution;
    solution.route = {0, 1, 2};
    solution.solution_size = 3;
    solution.total_time = 15;
    solution.total_distance = 45;
    solution.total_value = 30;

    std::string tempFile = std::string(DATA_DIR) + "/temp_solution.json";
    ASSERT_TRUE(JsonParser::WriteSolutionToJsonFile(tempFile, std::move(solution)));

    Solution readBack;
    ASSERT_TRUE(ParseSolutionFromJson(tempFile, readBack));

    EXPECT_EQ(readBack.route.size(), 3);
    EXPECT_EQ(readBack.solution_size, 3);
    EXPECT_EQ(readBack.total_time, 15);
    EXPECT_EQ(readBack.total_distance, 45);
    EXPECT_EQ(readBack.total_value, 30);

    std::remove(tempFile.c_str());
}
