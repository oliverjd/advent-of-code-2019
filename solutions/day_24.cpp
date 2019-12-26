#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <set>

using namespace std;

typedef vector<vector<bool>> Grid;


int64_t biodiversity(const Grid& vec) {
    vector<vector<int64_t>> bio_grid;
    int n = 5;
    for (int i = 0; i < n; ++i) {
        bio_grid.push_back({});
        for (int j = 0; j < n; ++j) {
            bio_grid[i].push_back(2 << (i * n + j - 1));
        }
    }
    bio_grid[0][0] = 1;

    int64_t sum {0};
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            sum += bio_grid[i][j] * vec[i][j];
        }
    }

    return sum;
}

void print_state(const Grid& vec) {
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            cout << vec[i][j];
        }
        cout << endl;
    }
}

Grid iterate(const Grid& vec) {
    auto new_vec {vec};
    int n = vec.size();
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            int adj_count {0};
            if (i > 0 && vec[i-1][j]) adj_count++;
            if (i < n-1 && vec[i+1][j]) adj_count++;
            if (j < n-1 && vec[i][j+1]) adj_count++;
            if (j > 0 && vec[i][j-1]) adj_count++;
            if (vec[i][j] && adj_count != 1) {
                new_vec[i][j] = false;
            } else if (!vec[i][j] && (adj_count == 1 || adj_count == 2)) {
                new_vec[i][j] = true;
            } else {
                new_vec[i][j] = vec[i][j];
            }
        }
    }
    return new_vec;
}

int part_1(const Grid& vec) {
    set<Grid> seen {vec};
    auto state {vec};
    Grid final_ans;
    while (true) {
        state = iterate(state);
        if (seen.find(state) != seen.end()) {
            final_ans = state;
            break;
        }
        seen.insert(state);
    }

    return biodiversity(final_ans);
}

typedef vector<Grid> Grid3d;

Grid3d iterate3d(const Grid3d& vec) {
    auto new_vec {vec};

    int D = vec.size();
    int N = vec[0].size();
    for (int d = 1; d < D - 1; ++d) {
        for (int i = 0; i < vec[0].size(); ++i) {
            for (int j = 0; j < vec[0].size(); ++j) {
                // d = depth, i/j = coords
                int adj_count {0};
                if (i == 2 && j == 2) { // middle square is not a square so ignore
                    continue;
                }
                if (i == 0) {
                    if (vec[d-1][1][2]) adj_count++;
                } else if (i == 4) {
                    if (vec[d-1][3][2]) adj_count++;
                }
                if (j == 0) {
                    if (vec[d-1][2][1]) adj_count++;
                } else if (j == 4) {
                    if (vec[d-1][2][3]) adj_count++;
                }
                if (i == 1 && j == 2) {
                    for (int h = 0; h < 5; ++h) {
                        if (vec[d+1][0][h]) adj_count++;
                    }
                } else if (i == 2 && j == 1) {
                    for (int h = 0; h < 5; ++h) {
                        if (vec[d+1][h][0]) adj_count++;
                    }
                } else if (i == 3 && j == 2) {
                    for (int h = 0; h < 5; ++h) {
                        if (vec[d+1][4][h]) adj_count++;
                    }
                } else if (i == 2 && j == 3) {
                    for (int h = 0; h < 5; ++h) {
                        if (vec[d+1][h][4]) adj_count++;
                    }
                }
                if (i > 0 && vec[d][i-1][j]) adj_count++;
                if (i < N-1 && vec[d][i+1][j]) adj_count++;
                if (j < N-1 && vec[d][i][j+1]) adj_count++;
                if (j > 0 && vec[d][i][j-1]) adj_count++;

                if (vec[d][i][j] && adj_count != 1) {
                    new_vec[d][i][j] = false;
                } else if (!vec[d][i][j] && (adj_count == 1 || adj_count == 2)) {
                    new_vec[d][i][j] = true;
                } else {
                    new_vec[d][i][j] = vec[d][i][j];
                }
            }

        }
    }
    return new_vec;
}

int part_2(const Grid& vec) {
    auto empty_grid {vec};
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            empty_grid[i][j] = false;
        }
    }

    int mins = 200;

    Grid3d g3;
    for (int i = 0; i < mins * 4; ++i) {
        g3.push_back(empty_grid);
    }
    g3[mins * 2] = vec;

    for (int i = 0; i < mins; ++i) {
        g3 = iterate3d(g3);
    }

    int64_t count = 0;
    for (int i = 0; i < g3.size(); ++i) {
        for (int j = 0; j < g3[0].size(); ++j) {
            for (int k = 0; k < g3[0][0].size(); ++k) {
                if (g3[i][j][k]) {
                    count++;
                }
            }
        }
    }

    return count;
}

int main() {
    ifstream in("../input/day_24.txt");
    string buf;
    Grid vec;
    while (getline(in, buf)) {
        vec.push_back({});
        for (int i = 0; i < buf.size(); ++i) {
            vec.back().push_back(buf[i] == '#');
        }
    }

    cout << part_1(vec) << endl;
    cout << part_2(vec) << endl;

    return 0;
}
