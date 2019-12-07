#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>

void run_program(std::vector<int>& reg, int &it, int &input_i, const std::vector<int>& inputs, bool& halted,
        int& output) {
    int op, opval, p1, p2, p3, m1, m2, m3, p1x, p2x, p3x;
    while (it < reg.size()) {
        opval = reg[it];
        op = opval % 100;
        m1 = (opval / 100) % 10;
        m2 = (opval / 1000) % 10;
        m3 = (opval / 10000) % 10;

        if (op != 99) {
            p1 = reg[it + 1];
            p1x = p1;
            if (m1 == 0) p1x = reg[p1];

            if (op <= 2 || op >= 5) {
                p2 = reg[it + 2];
                p2x = p2;
                if (m2 == 0) p2x = reg[p2];
            }
            if (op <= 2 || op >= 6) {
                p3 = reg[it + 3];
                p3x = p3;
                if (m3 == 0) p3x = reg[p3];
            }
        }

        if (op == 1) {
            reg[p3] = p1x + p2x;
            it += 4;
        } else if (op == 2) {
            reg[p3] = p1x * p2x;
            it += 4;
        } else if (op == 3) {
            if (input_i == inputs.size()) {
                // Awaiting next input
                return;
            }
            reg[p1] = inputs[input_i++];

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
            if (p1x < p2x) reg[p3] = 1;
            else reg[p3] = 0;
            it += 4;
        } else if (op == 8) {
            if (p1x == p2x) reg[p3] = 1;
            else reg[p3] = 0;
            it += 4;
        } else if (op == 99) {
            halted = true;
            return;
            break;
        } else {
            throw std::runtime_error("Unexpected opcode " + std::to_string(op));
        }
    }
    throw std::runtime_error("No return");
}

int run_on_input(std::vector<int> input, const std::vector<int>& prog, bool feedback) {
    int i = 0;
    int val = 0;
    int e_val = 0;
    int n = input.size();
    bool halted = false;
    std::vector<std::vector<int>> registers{(size_t)n};
    std::fill(registers.begin(), registers.end(), prog);

    std::vector<int> ins_ptrs(n);
    std::fill(ins_ptrs.begin(), ins_ptrs.end(), 0);

    std::vector<int> input_is(n);
    std::fill(input_is.begin(), input_is.end(), 0);

    std::vector<int> inputs {input[i], val};

    while (true) {
        inputs = {input[i], val};
        run_program(registers[i], ins_ptrs[i], input_is[i], inputs, halted, val);
        if (input_is[i] == inputs.size()) {
            input_is[i]--;
        }

        ++i;
        if (i%n == 0) {
            e_val = val;
        }
        if (!feedback && i == n) {
            break;
        }
        i %= n;
        if (halted) {
            break;
        }
    }
    return e_val;
}

int part_1(const std::vector<int>& vec) {
    const std::vector<int> start_phases {0,1,2,3,4};
    std::vector<int> phases = start_phases;
    std::next_permutation(phases.begin(), phases.end());

    int max_val = 0;
    while (phases != start_phases) {
        std::next_permutation(phases.begin(), phases.end());
        max_val = std::max(run_on_input(phases, vec, false), max_val);
    }

    return max_val;
}

int part_2(const std::vector<int>& vec) {
    const std::vector<int> start_phases {5,6,7,8,9};
    std::vector<int> phases = start_phases;
    std::next_permutation(phases.begin(), phases.end());

    int max_val = 0;
    while (phases != start_phases) {
        std::next_permutation(phases.begin(), phases.end());
        max_val = std::max(run_on_input(phases, vec, true), max_val);
    }

    return max_val;
}

int main() {
    std::ifstream in{"../input/day_07.txt"};
    std::string buf;
    std::vector<int> vec;
    while (std::getline(in, buf, ',')) {
        vec.push_back(std::stoi(buf));
    }

    std::cout << part_1(vec) << std::endl;
    std::cout << part_2(vec) << std::endl;

    return 0;
}
