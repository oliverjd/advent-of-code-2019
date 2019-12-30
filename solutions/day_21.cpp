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
    IntPuter(const vector<int64_t>& intcode, const vector<int64_t>& initial_input={}, const int64_t memsize=1000000) :
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
        memory_.resize(memsize, 0ll);
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

    string step_until_done() {
        string s;
        while (!awaiting_input() && !is_halted()) {
            step_forward();
            auto c = get_output();
            if (c > 127) {
                s += to_string(c);
            } else {
                s += c;
            }
        }
        if (awaiting_input()) {
            cout << "<AWAITING INPUT>" << endl;
        } else if (is_halted()) {
            cout << "<HALTED>" << endl;
        }
        return s;
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

vector<int64_t> to_ascii(string s) {
    vector<int64_t> vec;
    for (int i = 0; i < s.size(); ++i) {
        vec.push_back(s[i]);
    }
    return vec;
}

string from_ascii(vector<int64_t> ascii) {
    string s;
    for (int i = 0; i < ascii.size(); ++i) {
        s += ascii[i];
    }
    return s;
}

int64_t part_1(const std::vector<int64_t>& vec) {
    auto ip = IntPuter(vec);

    ip.step_until_done();
    vector<string> instructions = {
        "NOT C J",
        "AND D J",
        "NOT A T",
        "OR T J",
        "WALK"
    };
    string s;
    for (auto &i : instructions) {
        s += i + "\n";
    }
    ip.provide_input(to_ascii(s));
    while (!ip.is_halted()) {
        ip.step_forward();
    }
    return ip.get_output();
}

int64_t part_2(const std::vector<int64_t>& vec) {
    auto ip = IntPuter(vec);

    ip.step_until_done();
    vector<string> instructions = {
        "NOT C J",
        "AND D J",
        "AND H J",
        "NOT B T",
        "AND D T",
        "OR T J",
        "NOT A T",
        "OR T J",
        "RUN"
    };
    string s;
    for (auto &i : instructions) {
        s += i + "\n";
    }
    ip.provide_input(to_ascii(s));
    while (!ip.is_halted()) {
        ip.step_forward();
    }
    return ip.get_output();
}

int main() {
    std::ifstream in{"../input/day_21.txt"};
    std::string buf;
    std::vector<int64_t> vec;
    while (std::getline(in, buf, ',')) {
        vec.push_back(std::stoll(buf));
    }

    std::cout << part_1(vec) << std::endl;
    std::cout << part_2(vec) << std::endl;

    return 0;
}
