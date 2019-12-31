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
        awaiting_input_(false),
        provided_output_(false)
    {
        memory_.resize(memsize, 0ll);
    }

    void step_forward() {
        run_program(memory_, ins_ptr_, input_i_, input_, halted_, output_, relbase_, awaiting_input_, provided_output_);
    }

    bool is_halted() const {
        return halted_;
    }

    int64_t get_output() const {
        return output_;
    }

    bool is_output() const {
        return provided_output_;
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
    bool provided_output_;

    void run_program(std::vector<int64_t>& reg, int64_t &it, int64_t &input_i, const std::vector<int64_t>& inputs,
                    bool& halted, int64_t& output, int64_t& relbase, bool& awaiting_input, bool& provided_output) {
        awaiting_input = false;
        provided_output = false;
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
                provided_output = true;
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

int64_t network(const vector<int64_t>& vec, const int part) {
    const int n = 50;
    vector<IntPuter> ips;
    vector<int64_t> idle;
    for (int i = 0; i < n; ++i) {
        ips.emplace_back(vec);
        idle.push_back(0);
        auto& ip = ips[i];
        ip.provide_input({i});
        ip.step_forward();
    }

    vector<deque<pair<int64_t, int64_t>>> queues(n);
    pair<int64_t, int64_t> nat = {-1, -1};

    int64_t last_nat_y = -1;

    while (true) {
        for (int i = 0; i < n; ++i) {
            auto& ip = ips[i];
            auto& q = queues[i];

            while (!ip.awaiting_input()) {
                idle[i] = 0;
                int64_t x = -1, y = -1, a = -1;
                a = ip.get_output();
                ip.step_forward();
                x = ip.get_output();
                ip.step_forward();
                y = ip.get_output();
                ip.step_forward();
                if (a >= 0 && a < n) {
                    queues[a].push_back({x, y});
                }
                if (a == 255) {
                    if (part == 1) {
                        return y;
                    }
                    nat = {x, y};
                }
            }

            if (q.size() == 0) {
                ip.provide_input({-1});
                idle[i]++;
                ip.step_forward();
            } else {
                idle[i] = 0;
                while (q.size() > 0 && ip.awaiting_input()) {
                    auto [x, y] = q.front();
                    q.pop_front();
                    ip.provide_input({x, y});
                    ip.step_forward();
                }
            }
        }

        if (all_of(idle.begin(), idle.end(), [](int64_t x){return x > 1;})) {
            queues[0].push_back(nat);
            if (nat.second == last_nat_y && part == 2) {
                return nat.second;
            }
            last_nat_y = nat.second;
            for (int i = 0; i < idle.size(); ++i) {
                idle[i] = 0;
            }
        }
    }
}

int64_t part_1(const std::vector<int64_t>& vec) {
    return network(vec, 1);
}

int64_t part_2(const std::vector<int64_t>& vec) {
    return network(vec, 2);
}

int main() {
    std::ifstream in{"../input/day_23.txt"};
    std::string buf;
    std::vector<int64_t> vec;
    while (std::getline(in, buf, ',')) {
        vec.push_back(std::stoll(buf));
    }

    std::cout << part_1(vec) << std::endl;
    std::cout << part_2(vec) << std::endl;

    return 0;
}
