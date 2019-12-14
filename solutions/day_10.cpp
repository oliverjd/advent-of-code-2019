#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <iterator>
#include <set>
#include <map>
#include <algorithm>
#include <cmath>

using namespace std;

typedef pair<int, int> Coord; // <y, x>. second is x, first is y
typedef pair<Coord, double> CoordDist;

double find_angle(Coord a, Coord b) {
    double angle = atan2((b.second - a.second), -(b.first - a.first));
    double degrees = angle * 180 / M_PI;
    if (degrees < 0) {
        degrees += 360;
    }
    return degrees;
}

double distance(Coord a, Coord b) {
    return sqrt(pow((b.second - a.second), 2) + pow((b.first - a.first), 2));
}

void find_location(const set<Coord>& asteroids, Coord& out_loc, int& out_max) {
    int max_asteroids = 0;
    Coord location {0,0};

    for (const auto &a : asteroids) {
        set<double> angles;
        for (const auto &b : asteroids) {
            if (a != b) {
                angles.insert(find_angle(a, b));
            }
        }
        double num = static_cast<int>(angles.size());
        if (num > max_asteroids) {
            max_asteroids = num;
            location = a;
        }
    }

    out_loc = location;
    out_max = max_asteroids;
}

set<Coord> set_asteroids(const vector<vector<bool>>& vec) {
    set<Coord> asteroids;

    for (int y = 0; y < vec.size(); ++y) {
        for (int x = 0; x < vec[y].size(); ++x) {
            if (vec[y][x]) {
                asteroids.insert({y, x});
            }
        }
    }
    return asteroids;
}

int part_1(const vector<vector<bool>>& vec) {
    auto asteroids = set_asteroids(vec);

    int max_asteroids = 0;
    Coord monitor_loc {0,0};
    find_location(asteroids, monitor_loc, max_asteroids);

    return max_asteroids;
}

int part_2(const vector<vector<bool>>& vec) {
    auto asteroids = set_asteroids(vec);

    int max_asteroids = 0;
    Coord monitor_loc {0,0};
    find_location(asteroids, monitor_loc, max_asteroids);

    map<double, vector<CoordDist>> angle_asteroids;

    for (const auto &a : asteroids) {
        double angle = find_angle(monitor_loc, a);
        double dist = distance(monitor_loc, a);

        if (angle_asteroids.find(angle) == angle_asteroids.end()) {
            angle_asteroids[angle] = {};
        }
        angle_asteroids[angle].push_back({a, dist});
    }

    std::vector<Coord> destroy_order;

    for (auto& [angle, vec] : angle_asteroids) {
        sort(vec.begin(), vec.end(), [](CoordDist& a, CoordDist& b) { return a.second < b.second; });
        destroy_order.push_back(vec[0].first);
        vec.erase(vec.begin());
    }

    auto el = destroy_order[200 - 1];
    return 100 * el.second + el.first;
}

int main() {
    ifstream in("../input/day_10.txt");
    string buf;
    vector<vector<bool>> vec;
    while (std::getline(in, buf)) {
        vector<bool> line(buf.size());
        for (int i = 0; i < buf.size(); ++i) {
            if (buf[i] == '#') {
                line[i] = true;
            } else {
                line[i] = false;
            }
        }
        vec.push_back(line);
    }

    cout << part_1(vec) << endl;
    cout << part_2(vec) << endl;

    return 0;
}
