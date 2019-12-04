#include <iostream>
#include <fstream>
#include <vector>

int num_poss(int min, int max, bool any_adj) {
    std::vector<std::string> poss;
    std::string buf;
    
    for (int i = min; i <= max; ++i) {
        buf = std::to_string(i);

        if (buf.size() != 6) continue;

        char max_seen {'0'};
        bool decreases {false};
        char last_seen {'_'};
        bool adjacent {false};
        int matches {0};

        for (const auto &c : buf) {
            if (c < max_seen) {
                decreases = true;
                break;
            }
            max_seen = c;

            if (c == last_seen) {
                ++matches;
            } else {
                if (matches == 1 || any_adj && matches >= 1) {
                    adjacent = true;
                }
                matches = 0;
            }
            last_seen = c;
        }
        if (matches == 1 || any_adj && matches >= 1) {
            adjacent = true;
        }

        if (!decreases && adjacent) {
            poss.push_back(buf);
        }
    }

    return poss.size();
}

int part_1(int min, int max) {
    return num_poss(min, max, true);
}

int part_2(int min, int max) {
    return num_poss(min, max, false);
}

int main() {
    std::ifstream in("../input/day_04.txt");
    std::string buf;
    std::getline(in, buf);

    int min = std::stoi(buf.substr(0, buf.find('-')));
    int max = std::stoi(buf.substr(buf.find('-') + 1));

    std::cout << part_1(min, max) << std::endl;
    std::cout << part_2(min, max) << std::endl;

    return 0;
}
