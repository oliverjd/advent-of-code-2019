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
        memory_.resize(10000000, 0ll);
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

vector<vector<char>> create_grid(const vector<int64_t>& prog) {
    auto ip = IntPuter(prog);
    vector<char> out;
    while (!ip.is_halted()) {
        ip.step_forward();
        out.push_back(ip.get_output());
    }
    // cout << out.size() << endl;

    int i = 0;
    int j = 0;
    vector<vector<char>> grid = {{}};
    for (auto &c : out) {
        cout << c;
        grid[j].push_back(c);
        if (c == '\n') {
            i = 0;
            j++;
            grid.push_back({});
        } else {
            i++;
        }
    }

    return grid;
}

int64_t part_1(const std::vector<int64_t>& vec) {
    auto grid = create_grid(vec);

    vector<pair<int, int>> intersections;
    for (int i = 1; i < grid.size(); ++i) {
        for (int j = 1; j < grid[i].size(); ++j) {
            if (grid[i][j] == '#' && grid[i-1][j] == '#' && grid[i+1][j] == '#' && grid[i][j+1] == '#' && grid[i][j-1] == '#') {
                intersections.push_back({i, j});
            }
        }
    }

    int res = 0;
    for (const auto& [i, j] : intersections) {
        res += i * j;
    }

    return res;
}

typedef pair<int, int> Coord;

vector<int64_t> from_ascii(string ascii) {
    vector<int64_t> vec;
    for (int i = 0; i < ascii.size(); ++i) {
        vec.push_back(ascii[i]);
    }
    return vec;
}

void step_until_input(IntPuter& ip) {
    ip.step_forward();
    cout << char(ip.get_output());
    while (!ip.awaiting_input()) {
        ip.step_forward();
        cout << char(ip.get_output());
    }
}

int64_t part_2(const std::vector<int64_t>& vec) {
    // Found by hand...
    string meta = "A,B,B,C,B,C,B,C,A,A\n";
    string a = "L,6,R,8,L,4,R,8,L,12\n";
    string b = "L,12,R,10,L,4\n";
    string c = "L,12,L,6,L,4,L,4\n";

    auto prog {vec};
    prog[0] = 2;
    auto ip = IntPuter(prog);

    step_until_input(ip);
    ip.provide_input(from_ascii(meta));
    step_until_input(ip);
    ip.provide_input(from_ascii(a));
    step_until_input(ip);
    ip.provide_input(from_ascii(b));
    step_until_input(ip);
    ip.provide_input(from_ascii(c));
    step_until_input(ip);
    ip.provide_input(from_ascii("n\n"));

    while (!ip.is_halted()) {
        ip.step_forward();
        cout << ip.get_output() << " ";
        if (ip.get_output() > 1000) {
            cout << endl;
            return ip.get_output();
        }
    }
    return ip.get_output();
}

int main() {
    std::ifstream in{"../input/day_17.txt"};
    std::string buf;
    std::vector<int64_t> vec;
    while (std::getline(in, buf, ',')) {
        vec.push_back(std::stoll(buf));
    }

    std::cout << part_1(vec) << std::endl;
    std::cout << part_2(vec) << std::endl;

    return 0;
}
