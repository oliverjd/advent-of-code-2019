#include <iostream>
#include <fstream>
#include <vector>

int run_program(std::vector<int>& reg, int id) {
    int op, opval, p1, p2, p3, m1, m2, m3, p1x, p2x, p3x;

    int it = 0;
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
            reg[p1] = id;
            it += 2;
        } else if (op == 4) {
            if (p1x != 0) return p1x;
            it += 2;
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
            break;
        } else {
            throw std::runtime_error("Unexpected opcode " + std::to_string(op));
        }
    }
    throw std::runtime_error("No return");
}

int part_1(const std::vector<int>& vec) {
    std::vector<int> reg{vec};
    return run_program(reg, 1);
}


int part_2(const std::vector<int>& vec) {
    std::vector<int> reg{vec};
    return run_program(reg, 5);
}

int main() {
    std::ifstream in{"../input/day_05.txt"};
    std::string buf;
    std::vector<int> vec;
    while (std::getline(in, buf, ',')) {
        vec.push_back(std::stoi(buf));
    }

    std::cout << part_1(vec) << std::endl;
    std::cout << part_2(vec) << std::endl;

    return 0;
}
