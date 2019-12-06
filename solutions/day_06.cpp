#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <deque>

class Node {
public:
    Node (const std::string& name, std::shared_ptr<Node> orbits) : name_(name), orbits_(orbits) {}

    std::shared_ptr<Node> orbits() const {
        return orbits_;
    }

    std::vector<std::shared_ptr<Node>> orbitedBy() const {
        return orbitedBy_;
    }

    std::vector<std::shared_ptr<Node>> allLinks() const {
        auto vec = orbitedBy_;
        vec.emplace_back(orbits_);
        return vec;
    }

    std::string name() const {
        return name_;
    }

    void setOrbits(std::shared_ptr<Node> orbits) {
        orbits_ = orbits;
    }

    void addOrbitedBy(std::shared_ptr<Node> orbiter) {
        orbitedBy_.push_back(orbiter);
    }

    std::shared_ptr<Node> getOrbits() {
        return orbits_;
    }

private:
    std::string name_;
    std::shared_ptr<Node> orbits_;
    std::vector<std::shared_ptr<Node>> orbitedBy_;
};

class Chain {
public:
    Chain() {}

    void addLink(const std::string& orbiter, const std::string& orbits) {
        if (table_.find(orbits) == table_.end()) {
            table_.emplace(orbits, std::make_shared<Node>(orbits, nullptr));
        }
        if (table_.find(orbiter) == table_.end()) {
            table_.emplace(orbiter, std::make_shared<Node>(orbiter, nullptr));
        }
        table_[orbiter]->setOrbits(table_[orbits]);
        table_[orbits]->addOrbitedBy(table_[orbiter]);
    }

    std::vector<std::shared_ptr<Node>> getNodes() const {
        std::vector<std::shared_ptr<Node>> vec;
        for (auto &[k, v] : table_ ) {
            vec.emplace_back(v);
        }
        return vec;
    }

    const std::shared_ptr<Node> findNode(std::string name) const {
        if (table_.find(name) != table_.end()) {
            return table_.at(name);
        }
        return nullptr;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Node>> table_;
};

int part_1(const Chain& chain) {
    int count = 0;
    std::shared_ptr<Node> n = nullptr;
    for (const auto &it : chain.getNodes()) {
        n = it;
        while (true) {
            n = n->getOrbits();
            if (n == nullptr) {
                break;
            }
            count++;
        }
    }
    return count;
}

int part_2(const Chain& chain) {
    const auto youOrbit = chain.findNode("YOU")->orbits();
    const auto sanOrbit = chain.findNode("SAN")->orbits();

    std::unordered_map<std::string, int> seen;
    seen.emplace(youOrbit->name(), 1);
    std::deque<std::shared_ptr<Node>> queue {youOrbit};
    std::shared_ptr<Node> cur = nullptr;
    while (true) {
        cur = queue.front();
        queue.pop_front();

        for (const auto& it : cur->allLinks()) {
            if (it == nullptr) {
                continue;
            }
            if (it->name() == sanOrbit->name()) {
                return seen[cur->name()];
            }
            if (seen.find(it->name()) == seen.end()) {
                queue.push_back(it);
                seen.emplace(it->name(), seen[cur->name()] + 1);
            }
        }
    }
}

int main() {
    std::ifstream in("../input/day_06.txt");
    std::string buf;

    Chain chain;

    std::string a, b;
    int mid;
    while (std::getline(in, buf)) {
        mid = buf.find(')');
        a = buf.substr(0, mid);
        b = buf.substr(mid + 1);
        chain.addLink(b, a);
    }

    std::cout << part_1(chain) << std::endl;
    std::cout << part_2(chain) << std::endl;

    return 0;
}
