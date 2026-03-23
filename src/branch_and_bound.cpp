#include "branch_and_bound.hpp"
#include <algorithm>
#include <vector>
#include <cmath>
#include <functional>
#include <iostream>
#include <chrono>

const double BranchAndBound::INF = 1e18;
const int BranchAndBound::MAXN = 1000;

BranchAndBound::BranchAndBound(double time_limit_seconds_)
    : n(0), best(INF), time_limit_seconds(time_limit_seconds_), timeout(false),
      call_count(0), prune_count(0) {}

double BranchAndBound::reduce(std::vector<std::vector<double>>& mat) {
    size_t rows = mat.size();
    if (rows == 0) return 0.0;
    size_t cols = mat[0].size();
    double h = 0.0;

    for (size_t i = 0; i < rows; ++i) {
        double mn = INF;
        for (size_t j = 0; j < cols; ++j) mn = std::min(mn, mat[i][j]);
        if (mn == INF) return INF;
        if (mn > 0 && mn < INF/2) {
            h += mn;
            for (size_t j = 0; j < cols; ++j) {
                if (mat[i][j] < INF/2) mat[i][j] -= mn;
            }
        }
    }

    for (size_t j = 0; j < cols; ++j) {
        double mn = INF;
        for (size_t i = 0; i < rows; ++i) mn = std::min(mn, mat[i][j]);
        if (mn == INF) return INF;
        if (mn > 0 && mn < INF/2) {
            h += mn;
            for (size_t i = 0; i < rows; ++i) {
                if (mat[i][j] < INF/2) mat[i][j] -= mn;
            }
        }
    }
    return h;
}

std::vector<int> BranchAndBound::two_opt_improve(std::vector<int> tour, double& cost) {
    int m = (int)tour.size();
    if (m <= 2) return tour;
    bool improved = true;
    while (improved && !timeout) {
        improved = false;
        for (int i = 1; i + 2 < m && !timeout; ++i) {
            for (int j = i + 1; j + 1 < m && !timeout; ++j) {
                int a = tour[i-1], b = tour[i], c = tour[j], d = tour[j+1];
                if (a < 0 || b < 0 || c < 0 || d < 0) continue;
                if (a >= n || b >= n || c >= n || d >= n) continue;
                double before = dist[a][b] + dist[c][d];
                double after  = dist[a][c] + dist[b][d];
                if (after + 1e-12 < before) {
                    std::reverse(tour.begin() + i, tour.begin() + j + 1);
                    improved = true;
                }
                if (std::chrono::duration<double>(std::chrono::steady_clock::now() - start_time).count() >= time_limit_seconds) {
                    timeout = true;
                }
            }
        }
    }
    cost = tourCost(tour);
    return tour;
}

bool BranchAndBound::has_partial_subtour(const std::vector<std::pair<int,int>>& included) {
    if (included.empty()) return false;
    std::vector<int> next(n, -1);
    for (auto &p : included) next[p.first] = p.second;
    std::vector<int> vis(n, 0);
    for (int s = 0; s < n; ++s) {
        if (next[s] == -1) continue;
        int cur = s;
        while (cur != -1 && vis[cur] == 0) {
            vis[cur] = s+1;
            cur = next[cur];
        }
        if (cur != -1 && vis[cur] == s+1) {
            int len = 1;
            int it = next[cur];
            while (it != cur && it != -1) { ++len; it = next[it]; }
            if (len < n) return true;
        }
    }
    return false;
}

bool BranchAndBound::reconstructIfFull(const std::vector<std::pair<int,int>>& included, std::vector<int>& out_path, double real_cost) {
    if ((int)included.size() < n) return false;

    std::vector<int> next(n, -1), prev(n, -1);
    for (auto &p : included) {
        int u = p.first, v = p.second;
        if (u < 0 || u >= n || v < 0 || v >= n) return false;
        if (next[u] != -1) return false;
        if (prev[v] != -1) return false;
        next[u] = v;
        prev[v] = u;
    }

    for (int i = 0; i < n; ++i) {
        if (next[i] == -1 || prev[i] == -1) return false;
    }

    int start = 0;
    std::vector<int> tour;
    tour.reserve(n+1);
    int cur = start;
    for (int t = 0; t < n; ++t) {
        tour.push_back(cur);
        cur = next[cur];
    }
    if (cur != start) return false;
    tour.push_back(start);

    double total_cost = 0.0;
    for (int i = 0; i + 1 < (int)tour.size(); ++i) {
        int a = tour[i], b = tour[i+1];
        if (dist[a][b] >= INF/2) return false;
        total_cost += dist[a][b];
    }

    out_path = tour;
    if (total_cost < best) {
        best = total_cost;
        best_path = out_path;
    }
    return true;
}

double BranchAndBound::tourCost(const std::vector<int>& tour) {
    if (tour.size() < 2) return INF;
    double c = 0.0;
    for (size_t i = 0; i + 1 < tour.size(); ++i) {
        int a = tour[i], b = tour[i+1];
        if (a < 0 || a >= n || b < 0 || b >= n) return INF;
        c += dist[a][b];
    }
    return c;
}

void BranchAndBound::dfs(const std::vector<std::vector<double>>& mat,
                         double current_reduction,
                         double real_cost,
                         const std::vector<std::pair<int,int>>& included,
                         const std::vector<int>& active_row,
                         const std::vector<int>& active_col) {
    ++call_count;

    if (timeout) return;
    if (std::chrono::duration<double>(std::chrono::steady_clock::now() - start_time).count() >= time_limit_seconds) {
        timeout = true;
        return;
    }

    int size = (int)mat.size();

    double lower_bound_total = real_cost + current_reduction;
    if (lower_bound_total >= best - 1e-9) {
        ++prune_count;
        return;
    }

    if (size == 0) {
        std::vector<int> path;
        reconstructIfFull(included, path, real_cost);
        return;
    }

    int a = -1, b = -1;
    double max_penalty = -1.0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (mat[i][j] >= INF/2) continue;
            if (fabs(mat[i][j]) < 1e-12) {
                double row_min = INF;
                for (int jj = 0; jj < size; ++jj) if (jj != j) row_min = std::min(row_min, mat[i][jj]);
                double col_min = INF;
                for (int ii = 0; ii < size; ++ii) if (ii != i) col_min = std::min(col_min, mat[ii][j]);
                double penalty = (row_min >= INF/2 ? 0.0 : row_min) + (col_min >= INF/2 ? 0.0 : col_min);
                if (penalty > max_penalty) { max_penalty = penalty; a = i; b = j; }
            }
        }
    }
    if (a == -1 || b == -1) {
        double minval = INF;
        for (int i = 0; i < size; ++i) for (int j = 0; j < size; ++j)
            if (mat[i][j] < minval) { minval = mat[i][j]; a = i; b = j; }
        if (a == -1 || b == -1) return;
    }

    int u = active_row[a];
    int v = active_col[b];

    {
        std::vector<std::pair<int,int>> new_included = included;
        new_included.emplace_back(u, v);

        if (!has_partial_subtour(new_included)) {
            int new_size = size - 1;
            std::vector<std::vector<double>> new_mat(new_size, std::vector<double>(new_size, INF));
            std::vector<int> new_active_row; new_active_row.reserve(new_size);
            std::vector<int> new_active_col; new_active_col.reserve(new_size);
            for (int i = 0; i < size; ++i) if (i != a) new_active_row.push_back(active_row[i]);
            for (int j = 0; j < size; ++j) if (j != b) new_active_col.push_back(active_col[j]);

            int ii = 0;
            for (int i = 0; i < size; ++i) {
                if (i == a) continue;
                int jj = 0;
                for (int j = 0; j < size; ++j) {
                    if (j == b) continue;
                    new_mat[ii][jj] = mat[i][j];
                    ++jj;
                }
                ++ii;
            }

            int row_idx = -1, col_idx = -1;
            for (int k = 0; k < (int)new_active_row.size(); ++k) if (new_active_row[k] == v) { row_idx = k; break; }
            for (int k = 0; k < (int)new_active_col.size(); ++k) if (new_active_col[k] == u) { col_idx = k; break; }
            if (row_idx != -1 && col_idx != -1) new_mat[row_idx][col_idx] = INF;

            double new_real = real_cost + dist[u][v];
            double new_reduction = reduce(new_mat);
            double child_lb = new_reduction;
            double include_lower_bound = new_real + child_lb;

            if (include_lower_bound < best - 1e-9) {
                dfs(new_mat, child_lb, new_real, new_included, new_active_row, new_active_col);
                if (timeout) return;
            } else {
                ++prune_count;
            }
        } else {
            ++prune_count;
        }
    }

    if (timeout) return;

    {
        std::vector<std::vector<double>> mat_ex = mat;
        mat_ex[a][b] = INF;
        
        double child_lb = current_reduction + max_penalty;
        double exclude_lower_bound = real_cost + child_lb;

        if (exclude_lower_bound < best - 1e-9) {
            double excl_reduction = reduce(mat_ex);
            dfs(mat_ex, current_reduction + excl_reduction, real_cost, included, active_row, active_col);
            if (timeout) return;
        } else {
            ++prune_count;
        }
    }
}

std::vector<int> BranchAndBound::findOptimalRoute(const std::vector<std::vector<int64_t>>& distance_matrix,
                                                  std::vector<int> route) {
    n = (int)distance_matrix.size();
    if (n == 0) return {};
    dist.resize(n);
    for (int i = 0; i < n; ++i) {
        if ((int)distance_matrix[i].size() != n) {
            return {};
        }
        dist[i].resize(n);
        for (int j = 0; j < n; ++j){
            dist[i][j] = (double)distance_matrix[i][j];
        }
        dist[i][i] = INF;
    }

    std::vector<int> heuristic_tour = route;
    if (heuristic_tour.empty()) {
        heuristic_tour.resize(n + 1);
        for (int i = 0; i < n; ++i) heuristic_tour[i] = i;
        heuristic_tour[n] = 0;
    } else {
        if (heuristic_tour.front() != heuristic_tour.back()) heuristic_tour.push_back(heuristic_tour.front());
    }

    double heuristic_cost = tourCost(heuristic_tour);

#ifdef DEBUG
    std::cout << "Found start tour cost: " << heuristic_cost << std::endl;
#endif

    if (heuristic_cost >= INF/2) {
        heuristic_tour.clear();
        heuristic_tour.resize(n + 1);
        for (int i = 0; i < n; ++i) heuristic_tour[i] = i;
        heuristic_tour[n] = 0;
        heuristic_cost = tourCost(heuristic_tour);
    }

    best = heuristic_cost;
    best_path = heuristic_tour;

    start_time = std::chrono::steady_clock::now();
    timeout = false;
    call_count = prune_count = 0;

    if (heuristic_cost < INF/2) {
        best_path = two_opt_improve(best_path, best);
    }

#ifdef DEBUG
    std::cout << "Path imporoved by two_opt_improve " << std::endl;
#endif

    std::vector<std::vector<double>> mat = dist;
    double h = reduce(mat);

#ifdef DEBUG
    std::cout << "Dist matrix reduced" << std::endl;
#endif

    std::vector<int> active_row(n), active_col(n);
    for (int i = 0; i < n; ++i) { active_row[i] = i; active_col[i] = i; }

    std::vector<std::pair<int,int>> included;

#ifdef DEBUG
    std::cout << "Dfs started" << std::endl;
#endif

    dfs(mat, h, 0.0, included, active_row, active_col);

#ifdef DEBUG
    std::cout << "Dfs finished" << std::endl;
#endif


    if (best_path.empty()) return heuristic_tour;
    return best_path;
}