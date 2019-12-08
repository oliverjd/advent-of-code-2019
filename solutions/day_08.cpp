#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <numeric>
#include <map>
#include <limits>
#include <algorithm>

int layer_with_fewest(const std::vector<int>& vec, int x, int px_per_layer) {
    int layer = 0;
    int zeros = 0;
    int min_layer = 0;
    int min_zeros = std::numeric_limits<int>::max();
    for (int i = 0; i < vec.size() + 1; ++i) {
        if (i > 0 && i % px_per_layer == 0) {
            if (zeros < min_zeros) {
                min_layer = layer;
                min_zeros = zeros;
            }
            zeros = 0;
            layer = i / px_per_layer;

            if (i == vec.size()) break;
        }

        if (vec[i] == 0) ++zeros;
    }

    return min_layer;
}

int part_1(const std::vector<int>& vec, int h, int w) {
    int px_per_layer = h * w;
    int min_layer = layer_with_fewest(vec, 0, px_per_layer);

    std::map<int, int> layer_counts;
    for (int i = 0; i < 10; ++i) layer_counts[i] = 0;
    for (int i = min_layer * px_per_layer; i < (min_layer + 1) * px_per_layer; ++i) ++layer_counts[vec[i]];

    return layer_counts[1] * layer_counts[2];
}

int part_2(const std::vector<int>& vec, int h, int w) {
    int px_per_layer = h * w;

    std::vector<std::vector<std::string>> image(h);
    std::vector<std::string> empty_row(w);
    std::fill(empty_row.begin(), empty_row.end(), "x");
    std::fill(image.begin(), image.end(), empty_row);

    std::map<int, std::string> char_map {{0, " "}, {1, "\u25A0"}};

    for (int i = vec.size() - 1; i >= 0; --i) {
        int row = (i % px_per_layer) / w;
        int col = (i % px_per_layer) % w;

        if (vec[i] != 2) {
            image[row][col] = char_map[vec[i]];
        }
    }

    for (auto &row : image) {
        std::copy(row.begin(), row.end(), std::ostream_iterator<std::string>(std::cout));
        std::cout << std::endl;
    }

    return 0;
}

int main() {
    std::ifstream in("../input/day_08.txt");
    std::string buf;
    std::vector<int> vec;
    std::getline(in, buf);
    for (const auto &c : buf) {
        vec.push_back(std::stoi(std::string(1, c)));
    }

    int h = 6;
    int w = 25;
    std::cout << part_1(vec, h, w) << std::endl;
    std::cout << part_2(vec, h, w) << std::endl;

    return 0;
}
