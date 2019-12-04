#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <string>
#include <algorithm>

enum class Dir {
    Left,
    Right,
    Up,
    Down
};

struct Cmd {
    Cmd(Dir dir, int dist) : dir(dir), dist(dist) {}
    const Dir dir;
    const int dist;
};

typedef std::vector<Cmd> Wire;
typedef std::pair<int, int> Coord;
typedef std::pair<Coord, int> CoordPair;

int manhattan(const Coord& c) {
    return std::abs(c.first) + std::abs(c.second);
}

void follow_wire_steps(const Wire& wire, std::map<Coord, int>& seen, std::map<Coord, int>& other_seen,
        std::map<Coord, int>& inter) {
    Coord pos = {0, 0};
    seen[pos] = 0;
    int steps = 1;
    for (const auto &it : wire) {
        for (int i = 0; i < it.dist; ++i) {
            if (it.dir == Dir::Left) {
                pos.first--;
            } else if (it.dir == Dir::Right) {
                pos.first++;
            } else if (it.dir == Dir::Up) {
                pos.second++;
            } else if (it.dir == Dir::Down) {
                pos.second--;
            }
            if (seen.find(pos) == seen.end()) {
                seen[pos] = steps;
            }
            if (other_seen.find(pos) != other_seen.end()) {
                inter[pos] = steps + other_seen[pos];
            }
        ++steps;
        }
    }
}

int part_1(const std::vector<Wire>& vec) {
    std::map<Coord, int> seen, inter, new_seen;
    follow_wire_steps(vec[0], seen, inter, inter);
    follow_wire_steps(vec[1], new_seen, seen, inter);
    std::vector<CoordPair> inters(inter.begin(), inter.end());
    std::sort(inters.begin(), inters.end(), [](const CoordPair& a, const CoordPair& b) -> bool {
        return manhattan(a.first) < manhattan(b.first);
    });
    return manhattan(inters[0].first);
}

int part_2(const std::vector<Wire>& vec) {
    std::map<Coord, int> seen, inter, new_seen;
    follow_wire_steps(vec[0], seen, inter, inter);
    follow_wire_steps(vec[1], new_seen, seen, inter);
    std::vector<CoordPair> inters(inter.begin(), inter.end());
    std::sort(inters.begin(), inters.end(), [](const CoordPair& a, const CoordPair& b) -> bool {
        return a.second < b.second;
    });
    return inters[0].second;
}

int main() {
    std::unordered_map<char, Dir> dirs;
    dirs['L'] = Dir::Left;
    dirs['R'] = Dir::Right;
    dirs['U'] = Dir::Up;
    dirs['D'] = Dir::Down;

    std::ifstream in{"../input/day_03.txt"};
    std::string buf;
    std::vector<Wire> wires;
    while (std::getline(in, buf)) {
        wires.push_back({});
        int i = 0;
        while (i++ < buf.size()) {
            int n = 0;
            while (buf[i + n] != ',') {
                if (i + ++n == buf.size()) {
                    break;
                }
            }
            wires.back().emplace_back(dirs.at(buf[i-1]), std::stoi(buf.substr(i, n)));
            i += n + 1;
        }
    }

    std::cout << part_1(wires) << std::endl;
    std::cout << part_2(wires) << std::endl;

    return 0;
}
