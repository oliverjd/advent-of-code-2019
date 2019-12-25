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
        halted_(false),
        awaiting_input_(false)
    {
        memory_.resize(100000, 0ll);
    }

    void step_forward() {
        run_program(memory_, ins_ptr_, input_i_, input_, halted_, output_, relbase_, awaiting_input_);
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

    bool awaiting_input() const {
        return awaiting_input_;
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
    bool awaiting_input_;

    void run_program(std::vector<int64_t>& reg, int64_t &it, int64_t &input_i, const std::vector<int64_t>& inputs,
                    bool& halted, int64_t& output, int64_t& relbase, bool& awaiting_input) {
        awaiting_input = false;
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
                    awaiting_input = true;
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

int64_t part_1(const std::vector<int64_t>& vec) {
    const int n {50};
    vector<vector<bool>> grid;
    int count = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            auto ip = IntPuter(vec, {i, j});
            while (!ip.is_halted()) {
                ip.step_forward();
            }
            auto out = ip.get_output();
            count += out;
        }
    }
    return count;
}

int find_point(const vector<vector<bool>>& grid, const int x) {
    for (int i = 0; i < grid.size() - x; ++i) {
        for (int j = 0; j < grid[i].size() - x; ++j) {
            if (grid[i][j] && grid[i][j+x-1] && grid[i+x-1][j]) {
                return 10000 * i + j;
            }
        }
    }
    throw ("Not found - grid too small?");
}

int64_t part_2(const std::vector<int64_t>& vec) {
    const int n {1100};
    const int x {100};
    vector<vector<bool>> grid;
    for (int i = 0; i < n; ++i) {
        grid.push_back({});
        for (int j = 0; j < n; ++j) {
            auto ip = IntPuter(vec, {i, j});
            while (!ip.is_halted()) {
                ip.step_forward();
            }
            auto out = ip.get_output();
            grid.back().push_back(out);
        }
    }

    return find_point(grid, x);
}

int main() {
    std::ifstream in{"../input/day_19.txt"};
    std::string buf;
    std::vector<int64_t> vec;
    while (std::getline(in, buf, ',')) {
        vec.push_back(std::stoll(buf));
    }

    std::cout << part_1(vec) << std::endl;
    std::cout << part_2(vec) << std::endl;

    return 0;
}
