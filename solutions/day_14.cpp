#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <map>
#include <cmath>

using namespace std;

struct Reactant {
    Reactant(string s) {
        num = stoll(s.substr(0, s.find(' ')));
        name = s.substr(s.find(' ')+1);
    }

    Reactant(string s, int64_t num) : name(s), num(num) {}

    int64_t num;
    string name;
};

class Reaction {
public:
    Reaction(vector<Reactant> reactants, Reactant output) : inputs_(reactants), output_(output) {}

    Reactant output() const {
        return output_;
    }

    vector<Reactant> inputs() const {
        return inputs_;
    }

private:
    vector<Reactant> inputs_;
    Reactant output_;
};

bool anyNonOre(const vector<Reactant>& vec) {
    for (auto& r : vec) {
        if (r.name != "ORE" && r.num > 0) {
            return true;
        }
    }
    return false;
}

vector<Reactant> aggregate(const vector<Reactant>& inputs) {
    std::map<string, int64_t> totals;
    for (const auto& r : inputs) {
        if (totals.find(r.name) == totals.end()) {
            totals[r.name] = 0;
        }
        totals[r.name] += r.num;
    }

    vector<Reactant> newVec;
    for (const auto& [k, v] : totals) {
        if (v != 0) {
            newVec.emplace_back(k, v);
        }
    }
    return newVec;
}

int64_t calcOre(const vector<Reaction>& vec, int64_t fuelAmt) {
    map<string, Reaction> outputMap;
    for (const auto r : vec) {
        outputMap.emplace(r.output().name, r);
    }
    outputMap.emplace("ORE", Reaction({{"ORE", 1}}, {"ORE", 1}));

    auto& fuel = outputMap.at("FUEL");
    Reaction ourReaction(fuel);

    vector<Reactant> inputs(ourReaction.inputs());
    for (auto& r : inputs) {
        r.num *= fuelAmt;
    }

    while (anyNonOre(inputs)) {
        vector<Reactant> newInputs;
        for (auto& r : inputs) {
            auto& newReaction = outputMap.at(r.name);
            int64_t newMult = ceil(r.num * 1. / newReaction.output().num);
            for (auto& newr : newReaction.inputs()) {
                int64_t newNum = newr.num * newMult;
                newInputs.emplace_back(newr.name, newNum);
            }
            int64_t leftoverNum = newReaction.output().num * newMult - r.num;
            if (leftoverNum > 0) {
                newInputs.emplace_back(r.name, -leftoverNum);
            }
        }
        inputs = aggregate(newInputs);
    }

    auto totalOre = std::accumulate(inputs.begin(), inputs.end(), 0ll, [](int64_t sum, Reactant& x){
        if (x.name == "ORE") {
            return sum + x.num;
        }
        return sum;
    });
    return totalOre;
}

int64_t part_1(const vector<Reaction>& vec) {
    return calcOre(vec, 1);
}

int64_t part_2(const vector<Reaction>& vec) {
    const int64_t ore = 1000000000000;
    int64_t l = 0;
    int64_t r = ore;
    int64_t mid;

    while (r - l > 1) {
        mid = (l + r) / 2;
        auto actualOre = calcOre(vec, mid);
        if (actualOre > ore) {
            r = mid;
        } else {
            l = mid;
        }
    }
    return mid;
}

int main() {
    ifstream in("../input/day_14.txt");
    string buf;
    vector<Reaction> vec;
    while (getline(in, buf)) {
        int64_t lasti = 0;
        auto left = buf.substr(0, buf.find('='));
        auto right = buf.substr(buf.find('=') + 3);
        vector<Reactant> reactants;
        while (true) {
            auto i = left.find(',', lasti + 1);
            auto s = left.substr(lasti, i - lasti);
            lasti = i + 2;
            if (i == string::npos) {
                s = s.substr(0, s.size() - 1);
            }
            reactants.emplace_back(s);
            if (i == string::npos) {
                break;
            }
        }
        vec.emplace_back(reactants, Reactant(right));
    }

    cout << part_1(vec) << endl;
    cout << part_2(vec) << endl;

    return 0;
}
