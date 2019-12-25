#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <map>
#include <set>
#include <deque>

using namespace std;

typedef pair<int, int> Coord;
typedef pair<Coord, int> Coord3d;
typedef vector<vector<char>> Grid;

bool is_char(const char c) {
    return c >= 'A' && c <= 'Z';
}

string label(const Coord& pos, const Grid& grid) {
    if (grid[pos.first][pos.second] != '.') {
        return "";
    }
    if (pos.first > 1 && is_char(grid[pos.first - 1][pos.second]) && is_char(grid[pos.first - 2][pos.second])) {
        return string(1, grid[pos.first - 2][pos.second]) + string(1, grid[pos.first - 1][pos.second]);
    } else if (pos.first < grid.size() - 2 && is_char(grid[pos.first + 1][pos.second]) && is_char(grid[pos.first + 2][pos.second])) {
        return string(1, grid[pos.first + 1][pos.second]) + string(1, grid[pos.first + 2][pos.second]);
    } else if (pos.second < grid[pos.first].size() - 2 && is_char(grid[pos.first][pos.second + 1]) && is_char(grid[pos.first][pos.second + 2])) {
        return string(1, grid[pos.first][pos.second + 1]) + string(1, grid[pos.first][pos.second + 2]);
    } else if (pos.second > 1 && is_char(grid[pos.first][pos.second - 1]) && is_char(grid[pos.first][pos.second - 2])) {
        return string(1, grid[pos.first][pos.second - 2]) + string(1, grid[pos.first][pos.second - 1]);
    }

    return "";
}

vector<Coord> get_moves(const Coord& pos, const Grid& grid, const map<Coord, Coord>& portals) {
    vector<Coord> v;

    if (portals.find({pos.first, pos.second}) != portals.end()) {
        v.push_back(portals.at({pos.first, pos.second}));
    }

    if (grid[pos.first + 1][pos.second] == '.') {
        v.emplace_back(pos.first + 1, pos.second);
    }
    if (grid[pos.first - 1][pos.second] == '.') {
        v.emplace_back(pos.first - 1, pos.second);
    }
    if (grid[pos.first][pos.second + 1] == '.') {
        v.emplace_back(pos.first, pos.second + 1);
    }
    if (grid[pos.first][pos.second - 1] == '.') {
        v.emplace_back(pos.first, pos.second - 1);
    }

    return v;
}

int bfs(const Grid& grid, const Coord& start, const Coord& end, const map<Coord, Coord>& portals) {
    map<Coord, int> seen;
    deque<pair<Coord, int>> q;

    for (const auto& move : get_moves(start, grid, portals)) {
        q.push_back({move, 1});
        seen[move] = 1;
    }

    while (q.size() > 0) {
        auto& f = q.front();
        for (const auto& move : get_moves(f.first, grid, portals)) {
            if (seen.find(move) != seen.end() && f.second + 1 >= seen[move]) {
                continue;
            }
            seen[move] = f.second + 1;
            q.push_back({move, f.second + 1});
        }

        q.pop_front();
    }

    return seen[end];
}

int part_1(const Grid& vec) {
    Coord start = {-1, -1};
    Coord end = {-1, -1};
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            if (label({i, j}, vec) == "AA") {
                start = {i, j};
            } else if (label({i, j}, vec) == "ZZ") {
                end = {i, j};
            }
        }
    }

    map<Coord, Coord> portals;
    map<string, Coord> portal_first_half;

    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            Coord p {i, j};
            const auto l = label(p, vec);
            if (l == "" or l == "AA" or l == "ZZ") {
                continue;
            }
            if (portal_first_half.find(l) == portal_first_half.end()) {
                portal_first_half[l] = {i, j};
            } else {
                portals[portal_first_half[l]] = {i, j};
                portals[{i, j}] = portal_first_half[l];
            }
        }
    }

    return bfs(vec, start, end, portals);
}

vector<Coord3d> get_moves3d(const Coord3d& pos3d, const Grid& grid, const map<Coord, Coord>& portals, const set<Coord>& outer) {
    vector<Coord3d> v;
    auto pos = pos3d.first;
    auto depth = pos3d.second;

    if (portals.find({pos.first, pos.second}) != portals.end()) {
        if (outer.find(pos) != outer.end() && depth > 0) {
            v.push_back({portals.at({pos.first, pos.second}), depth - 1});
        }
        if (outer.find(pos) == outer.end()) {
            v.push_back({portals.at({pos.first, pos.second}), depth + 1});
        }
    }

    if (grid[pos.first + 1][pos.second] == '.') {
        v.push_back({{pos.first + 1, pos.second}, depth});
    }
    if (grid[pos.first - 1][pos.second] == '.') {
        v.push_back({{pos.first - 1, pos.second}, depth});
    }
    if (grid[pos.first][pos.second + 1] == '.') {
        v.push_back({{pos.first, pos.second + 1}, depth});
    }
    if (grid[pos.first][pos.second - 1] == '.') {
        v.push_back({{pos.first, pos.second - 1}, depth});
    }

    return v;
}

int bfs3d(const Grid& grid, const Coord& start, const Coord& end, const map<Coord, Coord>& portals, const set<Coord>& outer) {
    int max_depth = 100; // needs to be chosen large enough depending on input
    map<Coord3d, int> seen;
    deque<pair<Coord3d, int>> q;

    for (const auto& move : get_moves3d({start, 0}, grid, portals, outer)) {
        q.push_back({move, 1});
        seen[move] = 1;
    }

    while (q.size() > 0) {
        auto& f = q.front();
        auto depth = f.first.second;
        for (const auto& move : get_moves3d(f.first, grid, portals, outer)) {
            if (seen.find(move) != seen.end() && f.second + 1 >= seen[move] || depth > max_depth) {
                continue;
            }
            seen[move] = f.second + 1;
            q.push_back({move, f.second + 1});
        }

        q.pop_front();
    }

    return seen[{end, 0}];
}

int part_2(const Grid& vec) {
    set<Coord> outer;

    Coord start = {-1, -1};
    Coord end = {-1, -1};
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            if (label({i, j}, vec) == "AA") {
                start = {i, j};
            } else if (label({i, j}, vec) == "ZZ") {
                end = {i, j};
            }

            if (i == 2 || i == 102 || j == 2 || j == 106) { // picked out from maze manually
                outer.insert({i, j});
            }
        }
    }

    map<Coord, Coord> portals;
    map<string, Coord> portal_first_half;

    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            Coord p {i, j};
            const auto l = label(p, vec);
            if (l == "" or l == "AA" or l == "ZZ") {
                continue;
            }
            if (portal_first_half.find(l) == portal_first_half.end()) {
                portal_first_half[l] = {i, j};
            } else {
                portals[portal_first_half[l]] = {i, j};
                portals[{i, j}] = portal_first_half[l];
            }
        }
    }

    return bfs3d(vec, start, end, portals, outer);
    return 0;
}

int main() {
    ifstream in("../input/day_20.txt");
    string buf;
    Grid vec;
    while (getline(in, buf)) {
        vec.push_back({});
        for (int i = 0; i < buf.size(); ++i) {
            vec.back().push_back(buf[i]);
        }
    }

    cout << part_1(vec) << endl;
    cout << part_2(vec) << endl;

    return 0;
}
