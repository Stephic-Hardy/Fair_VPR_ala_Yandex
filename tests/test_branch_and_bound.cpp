#include <gtest/gtest.h>
#include "../src/branch_and_bound.hpp"

double calculate_cost(const std::vector<std::vector<double>>& dist, const std::vector<int>& path) {
    double cost = 0.0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        cost += dist[path[i]][path[i + 1]];
    }
    return cost;
}

TEST(BranchAndBoundTest, BasicTSP1) {
    std::vector<std::vector<double>> dist = {
        {0, 900, 600, 2400, 1100, 1000},
        {900, 0, 600, 2000, 720, 800},
        {600, 600, 0, 1900, 1100, 1300},
        {2400, 2000, 1900, 0, 2700, 2800},
        {1100, 720, 1100, 2700, 0, 120},
        {1000, 800, 1300, 2800, 120, 0}
    };

    BranchAndBound bb;
    std::vector<int> route = bb.findOptimalRoute(dist);

    
    ASSERT_EQ(route.front(), 0);
    ASSERT_EQ(route.back(), 0);
    ASSERT_EQ(route.size(), dist.size() + 1); 

    std::set<int> visited;
    for (size_t i = 1; i < route.size() - 1; ++i) {
        visited.insert(route[i]);
    }
    ASSERT_EQ(visited.size(), dist.size() - 1);  

    double cost = calculate_cost(dist, route);
    EXPECT_DOUBLE_EQ(cost, 6340.0);  
}

TEST(BranchAndBoundTest, BasicTSP2) {
    std::vector<std::vector<double>> dist = {
            {0, 27, 41, 16, 30, 26},
            {7, 0, 16, 1, 30, 25},
            {20, 13, 0, 35, 5, 0},
            {21, 16, 25, 0, 18, 18},
            {12, 46, 27, 48, 0, 5},
            {25, 5, 5, 9, 5, 0}
    };

    BranchAndBound bb;
    std::vector<int> route = bb.findOptimalRoute(dist);

    
    ASSERT_EQ(route.front(), 0);
    ASSERT_EQ(route.back(), 0);
    ASSERT_EQ(route.size(), dist.size() + 1); 

    std::set<int> visited;
    for (size_t i = 1; i < route.size() - 1; ++i) {
        visited.insert(route[i]);
    }
    ASSERT_EQ(visited.size(), dist.size() - 1);  

    double cost = calculate_cost(dist, route);
    EXPECT_DOUBLE_EQ(cost, 63.0);  
}

TEST(BranchAndBoundTest, SmallMatrix) {
    std::vector<std::vector<double>> dist = {
        {0, 10},
        {10, 0}
    };

    BranchAndBound bb;
    std::vector<int> route = bb.findOptimalRoute(dist);

    ASSERT_EQ(route, std::vector<int>({0, 1, 0}));
    double cost = calculate_cost(dist, route);
    EXPECT_DOUBLE_EQ(cost, 20.0);
}

TEST(BranchAndBoundTest, InvalidInput) {
    std::vector<std::vector<double>> dist = {{0}}; 
    BranchAndBound bb;
    std::vector<int> route = bb.findOptimalRoute(dist);
    ASSERT_EQ(route, std::vector<int>({0, 0}));
}