#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <map>
#include <deque>
#include <iterator>
#include <set>

using namespace std;

class IntPuter {
public:
    IntPuter(const vector<int64_t>& intcode, const vector<int64_t>& initial_input={}) :
        intcode_(intcode),
        memory_(intcode),
        input_(initial_input),
        input_i_(0),
        ins_ptr_(0),
        output_(0),
        relbase_(0),
        halted_(false)
    {
        memory_.resize(100000, 0ll);
    }

    void step_forward() {
        run_program(memory_, ins_ptr_, input_i_, input_, halted_, output_, relbase_);
    }

    bool is_halted() const {
        return halted_;
    }

    int64_t get_output() const {
        return output_;
    }

    void provide_input(const vector<int64_t>& input) {
        input_ = input;
        input_i_ = 0;
    }

private:
    const vector<int64_t> intcode_;
    vector<int64_t> memory_;
    vector<int64_t> input_;
    int64_t input_i_;
    int64_t ins_ptr_;
    int64_t output_;
    int64_t relbase_;
    bool halted_;

    void run_program(std::vector<int64_t>& reg, int64_t &it, int64_t &input_i, const std::vector<int64_t>& inputs,
                    bool& halted, int64_t& output, int64_t& relbase) {
        int64_t op, opval, p1, p2, p3, m1, m2, m3, p1x, p2x, p3x, p1i, p2i, p3i;
        while (it < reg.size()) {
            opval = reg.at(it);
            op = opval % 100;
            m1 = (opval / 100) % 10;
            m2 = (opval / 1000) % 10;
            m3 = (opval / 10000) % 10;

            if (op != 99) {
                p1 = reg.at(it + 1);
                p1x = p1;
                p1i = p1;
                if (m1 == 0) {
                    p1x = reg.at(p1);
                } else if (m1 == 2) {
                    p1x = reg.at(p1 + relbase);
                    p1i += relbase;
                }

                if (op == 1 || op == 2 || op == 5 || op == 6 || op == 7 || op == 8) {
                    p2 = reg.at(it + 2);
                    p2x = p2;
                    p2i = p2;
                    if (m2 == 0) {
                        p2x = reg.at(p2);
                    } else if (m2 == 2) {
                        p2x = reg.at(p2 + relbase);
                        p2i += relbase;
                    }
                }
                if (op == 1 || op == 2 || op == 6 || op == 7 || op == 8) {
                    p3 = reg.at(it + 3);
                    p3x = p3;
                    p3i = p3;
                    if (m3 == 0) {
                        p3x = reg.at(p3);
                    } else if (m3 == 2) {
                        p3x = reg.at(p3 + relbase);
                        p3i += relbase;
                    }
                }
            }

            if (op == 1) {
                reg[p3i] = p1x + p2x;
                it += 4;
            } else if (op == 2) {
                reg[p3i] = p1x * p2x;
                it += 4;
            } else if (op == 3) {
                if (input_i == inputs.size()) {
                    // Awaiting next input
                    return;
                }
                reg[p1i] = inputs.at(input_i++);

                it += 2;
            } else if (op == 4) {
                it += 2;
                output = p1x;
                return;
            } else if (op == 5) {
                if (p1x != 0) it = p2x;
                else it += 3;
            } else if (op == 6) {
                if (p1x == 0) it = p2x;
                else it += 3;
            } else if (op == 7) {
                if (p1x < p2x) reg[p3i] = 1;
                else reg[p3i] = 0;
                it += 4;
            } else if (op == 8) {
                if (p1x == p2x) reg[p3i] = 1;
                else reg[p3i] = 0;
                it += 4;
            } else if (op == 9) {
                relbase += p1x;
                it += 2;
            } else if (op == 99) {
                halted = true;
                return;
            } else {
                throw std::runtime_error("Unexpected opcode " + std::to_string(op));
            }
        }
        throw std::runtime_error("No return");
    }
};

typedef pair<int64_t, int64_t> Coord; // x, y
typedef int64_t Steps;

enum class Dir {
    NORTH = 1ll,
    SOUTH,
    WEST,
    EAST,
};

enum class Res {
    WALL = 0ll,
    FREE,
    FOUND,
    FILLED
};

Coord move(Coord pos, Dir dir) {
    switch (dir) {
        case Dir::NORTH:
            pos.first++;
            break;
        case Dir::SOUTH:
            pos.first--;
            break;
        case Dir::WEST:
            pos.second--;
            break;
        case Dir::EAST:
            pos.second++;
            break;
        default:
            throw runtime_error("Direction wrong");
    }
    return pos;
}

typedef map<Coord, tuple<Steps, Res>> Map;

void find_o2(const vector<int64_t>& vec, Coord& pos_out, int64_t& steps_out, Map& map_out) {
    Map seen;
    deque<tuple<IntPuter, Coord, Steps>> state_queue;
    Coord found_pos;
    bool found = false;

    for (int i = 1; i <= 4; ++i) {
        IntPuter ip(vec, {i});
        ip.step_forward();
        auto out = ip.get_output();
        seen[{0, 0}] = {0, Res::FREE};
        Coord pos = move({0,0}, Dir(i));
        seen[pos] = {1, Res(out)};
        if (Res(out) != Res::WALL) {
            state_queue.push_back({ip, pos, 1});
        }
    }

    while (state_queue.size() > 0) {
        auto [ip_ori, pos_ori, steps_ori] = state_queue.front();
        state_queue.pop_front();
        for (int i = 1; i <= 4; ++i) {
            auto ip = ip_ori;
            auto pos = pos_ori;
            auto steps = steps_ori;
            ip.provide_input({i});
            ip.step_forward();
            auto out = ip.get_output();
            ++steps;
            Coord new_pos = move(pos, Dir(i));
            if (seen.find(new_pos) == seen.end() or steps < get<0>(seen[new_pos])) {
                seen[new_pos] = {steps, Res(out)};
                if (Res(out) != Res::WALL) {
                    state_queue.push_back({ip, new_pos, steps});
                }
                if (Res(out) == Res::FOUND) {
                    found = true;
                    found_pos = new_pos;
                }
            }
        }
    }

    if (!found) {
        throw runtime_error("Not found");
    }

    pos_out = found_pos;
    steps_out = get<0>(seen.at(found_pos));
    map_out = seen;
}

int64_t part_1(const std::vector<int64_t>& vec) {
    int64_t steps;
    Coord found;
    Map area_map;
    find_o2(vec, found, steps, area_map);
    return steps;
}

typedef map<Coord, Res> SimpleMap;

int64_t part_2(const std::vector<int64_t>& vec) {
    int64_t steps;
    Coord found_pos;
    Map area_map;
    find_o2(vec, found_pos, steps, area_map);

    SimpleMap area_simple;
    set<Coord> unfilled;
    for (const auto& [k, v] : area_map) {
        area_simple[k] = get<1>(v);
        if (get<1>(v) == Res::FREE) {
            unfilled.insert(k);
        }
    }
    area_simple[found_pos] = Res::FILLED;

    int64_t time = 0;
    while (unfilled.size() > 0) {
        auto area_new {area_simple};
        for (auto& [k, v] : area_simple) {
            if (v == Res::FILLED) {
                vector<Coord> poss = {{k.first, k.second + 1}, {k.first, k.second - 1},
                                      {k.first + 1, k.second}, {k.first - 1, k.second}};
                for (auto& pos : poss) {
                    if (area_new.find(pos) != area_new.end() && area_new[pos] == Res::FREE) {
                        area_new[pos] = Res::FILLED;
                        unfilled.erase(pos);
                    }
                }
            }
        }
        area_simple = area_new;
        ++time;
    }

    return time;
}

int main() {
    std::ifstream in{"../input/day_15.txt"};
    std::string buf;
    std::vector<int64_t> vec;
    while (std::getline(in, buf, ',')) {
        vec.push_back(std::stoll(buf));
    }

    std::cout << part_1(vec) << std::endl;
    std::cout << part_2(vec) << std::endl;

    return 0;
}
