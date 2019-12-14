#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>

void run_program(std::vector<int64_t>& reg, int64_t &it, int64_t &input_i, const std::vector<int64_t>& inputs, bool& halted,
        int64_t& output, int64_t& relbase) {
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

int64_t run_on_input(std::vector<int64_t> input, const std::vector<int64_t>& prog) {
    int64_t i {0};
    int64_t output {0};
    int64_t e_val {0};
    int64_t relbase {0};
    int64_t n {static_cast<int64_t>(input.size())};
    bool halted {false};

    std::vector<int64_t> reg(prog);
    reg.resize(100000000, 0ll);

    int64_t ins_ptr {0};
    int64_t input_i {0};

    while (!halted) {
        run_program(reg, ins_ptr, input_i, input, halted, output, relbase);
    }

    return output;
}

int64_t part_1(const std::vector<int64_t>& vec) {
    return run_on_input({1}, vec);
}

int64_t part_2(const std::vector<int64_t>& vec) {
    return run_on_input({2}, vec);
}

int main() {
    std::ifstream in{"../input/day_09.txt"};
    std::string buf;
    std::vector<int64_t> vec;
    while (std::getline(in, buf, ',')) {
        vec.push_back(std::stoll(buf));
    }

    std::cout << part_1(vec) << std::endl;
    std::cout << part_2(vec) << std::endl;

    return 0;
}
