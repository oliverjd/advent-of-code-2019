#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>

int calc_fuel(int mass) {
    return std::max(mass/3 - 2, 0);
}

int calc_fuel_inc_fuel(int mass) {
    int fuel = calc_fuel(mass);
    int this_fuel = fuel;

    while (this_fuel > 0) {
        this_fuel = calc_fuel(this_fuel);
        fuel += this_fuel;
    }

    return fuel;
}

int part_1(const std::vector<int>& vec) {
    return std::accumulate(vec.begin(), vec.end(), 0, [](int tot, int mass) { return tot + calc_fuel(mass); });
}

int part_2(const std::vector<int>& vec) {
    return std::accumulate(vec.begin(), vec.end(), 0, [](int tot, int mass) { return tot + calc_fuel_inc_fuel(mass); });
}

int main() {
    std::ifstream in("../input/day_01.txt");
    std::string buf;
    std::vector<int> vec;
    while (std::getline(in, buf)) {
        vec.push_back(std::stoi(buf));
    }

    std::cout << part_1(vec) << std::endl;
    std::cout << part_2(vec) << std::endl;

    return 0;
}
