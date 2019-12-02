#include <iostream>
#include <fstream>
#include <vector>

void run_program(std::vector<int>& reg, int noun, int verb) {
    reg[1] = noun;
    reg[2] = verb;

    int op, a, b, o;
    auto it {reg.begin()};
    while (it < reg.end()) {
        op = *it;
        a = *(it + 1);
        b = *(it + 2);
        o = *(it + 3);

        if (op == 1) {
            reg[o] = reg[a] + reg[b];
            it += 4;
        } else if (op == 2) {
            reg[o] = reg[a] * reg[b];
            it += 4;
        } else if (op == 99) {
            break;
        } else {
            throw std::runtime_error("Unexpected opcode");
        }
    }
}

int part_1(const std::vector<int>& vec) {
    std::vector<int> reg{vec};
    run_program(reg, 12, 2);
    return reg[0];
}


int part_2(const std::vector<int>& vec) {
    const int target{19690720};

    for (int noun = 0; noun <= 99; ++noun) {
        for (int verb = 0; verb <= 99; ++verb) {
            std::vector<int> reg{vec};
            run_program(reg, noun, verb);
            if (reg[0] == target) {
                return 100 * noun + verb;
            }
        }
    }

    throw std::runtime_error("Target not found");
}

int main() {
    std::ifstream in{"../input/day_02.txt"};
    std::string buf;
    std::vector<int> vec;
    while (std::getline(in, buf, ',')) {
        vec.push_back(std::stoi(buf));
    }

    std::cout << part_1(vec) << std::endl;
    std::cout << part_2(vec) << std::endl;

    return 0;
}
