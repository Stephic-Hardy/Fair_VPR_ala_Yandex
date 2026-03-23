
#ifndef BRANCH_AND_BOUND_HPP
#define BRANCH_AND_BOUND_HPP

#include <algorithm>
#include <vector>
#include <cmath>
#include <functional>
#include <iostream>
#include <chrono>
#include <vector>

class BranchAndBound {
public:
    static const double INF;
    static const int MAXN;

    BranchAndBound(double time_limit_seconds = 5);

    std::vector<int> findOptimalRoute(const std::vector<std::vector<int64_t>>& distance_matrix,
                                      std::vector<int> route = {});
    std::vector<int> findOptimalRoute2(const std::vector<std::vector<double>>& distance_matrix,
                                                  std::vector<int> route);


    std::vector<int> two_opt_improve(std::vector<int> tour, double& cost);

private:
    int n;
    std::vector<std::vector<double>> dist;

    double best;
    std::vector<int> best_path;

    double time_limit_seconds;
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    bool timeout;

    long long call_count;
    long long prune_count;


    double reduce(std::vector<std::vector<double>>& mat);

    bool has_cycle(const std::vector<int>& next_city);


    bool has_partial_subtour(const std::vector<std::pair<int,int>>& included);


    bool reconstructIfFull(const std::vector<std::pair<int,int>>& included, std::vector<int>& out_path, double real_cost);

    double tourCost(const std::vector<int>& tour);


void dfs(const std::vector<std::vector<double>>& mat,
                         double current_reduction,
                         double real_cost,
                         const std::vector<std::pair<int,int>>& included,
                         const std::vector<int>& active_row,
                         const std::vector<int>& active_col);
};

#endif 
