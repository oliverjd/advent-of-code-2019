#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>

using namespace std;

class Moon{
public:
    Moon(int64_t x, int64_t y, int64_t z) : x_(x), y_(y), z_(z), xdot_(0), ydot_(0), zdot_(0) {}

    int64_t energy() const {
        auto pot = potential_energy();
        auto kin = kinetic_energy();
        return pot * kin;
    }

    void iteratePos() {
        x_ += xdot_;
        y_ += ydot_;
        z_ += zdot_;
    }

    int64_t x() const {
        return x_;
    }

    int64_t y() const {
        return y_;
    }

    int64_t z() const {
        return z_;
    }

    int64_t xdot() const {
        return xdot_;
    }

    int64_t ydot() const {
        return ydot_;
    }

    int64_t zdot() const {
        return zdot_;
    }

    void incrementVel(int64_t x, int64_t y, int64_t z) {
        xdot_ += x;
        ydot_ += y;
        zdot_ += z;
    }

    void printState() const {
        cout << "pos=<x=" << x_ << ", y=" << y_ << ", z=" << z_ << ">, vel=<x=" << xdot_
             << ", y=" << ydot_ << ", z=" << zdot_ << ">" << endl;
    }

private:
    int64_t x_;
    int64_t y_;
    int64_t z_;

    int64_t xdot_;
    int64_t ydot_;
    int64_t zdot_;

    int64_t potential_energy() const {
        return abs(x_) + abs(y_) + abs(z_);
    }

    int64_t kinetic_energy() const {
        return abs(xdot_) + abs(ydot_) + abs(zdot_);
    }
};

void iterate(vector<Moon>& moons, bool x, bool y, bool z) {
    for (int64_t i = 0; i < moons.size(); ++i) {
        for (int64_t j =0; j < i; ++j) {
            if (i != j) {
                Moon& a = moons[i];
                Moon& b = moons[j];

                if (x) {
                    if (a.x() < b.x()) {
                        a.incrementVel(1, 0, 0);
                        b.incrementVel(-1, 0, 0);
                    } else if (a.x() > b.x()) {
                        a.incrementVel(-1, 0, 0);
                        b.incrementVel(1, 0, 0);
                    }
                }
                if (y) {
                    if (a.y() < b.y()) {
                        a.incrementVel(0, 1, 0);
                        b.incrementVel(0, -1, 0);
                    } else if (a.y() > b.y()) {
                        a.incrementVel(0, -1, 0);
                        b.incrementVel(0, 1, 0);
                    }
                }
                if (z) {
                    if (a.z() < b.z()) {
                        a.incrementVel(0, 0, 1);
                        b.incrementVel(0, 0, -1);
                    } else if (a.z() > b.z()) {
                        a.incrementVel(0, 0, -1);
                        b.incrementVel(0, 0, 1);
                    }
                }
            }
        }
    }

    for (auto& moon : moons) {
        moon.iteratePos();
    }

}

int64_t total_energy(vector<Moon>& moons) {
    return accumulate(moons.begin(), moons.end(), 0, [](int64_t a, Moon& b){ return a + b.energy(); });
}

bool equal(const Moon& a, const Moon& b) {
    return (a.x() == b.x() && a.y() == b.y() && a.z() == b.z()
            && a.xdot() == b.xdot() && a.ydot() == b.ydot() && a.zdot() == b.zdot());
}

int64_t part_1(const vector<Moon>& vec) {
    auto moons(vec);
    for (int64_t i = 1; i <= 1000; ++i) {
        iterate(moons, true, true, true);
    }

    return total_energy(moons);
}

int64_t numIters(const vector<Moon>& moonsIn, bool x, bool y, bool z) {
    int64_t its = 0;
    auto moons(moonsIn);
    auto initMoons(moonsIn);

    bool all_equal;
    while (true) {
        ++its;
        iterate(moons, x, y, z);

        all_equal = true;
        for (int i = 0; i < moons.size(); ++i) {
            if (!equal(moons[i], initMoons[i])) {
                all_equal = false;
            }
        }
        if (all_equal) {
            break;
        }

    }
    return its;

}

int64_t lcm(int64_t a, int64_t b) {
   int64_t max, step, lcm;

   lcm = 0;
   if(a > b) {
      max = step = a;
   } else {
      max = step = b;
   }

   while(true) {
      if(max % a == 0 && max % b == 0) {
         lcm = max;
         break;
      }
      max += step;
   }

   return lcm;
}

int64_t part_2(const vector<Moon>& vec) {
    int64_t x_iters = numIters(vec, true, false, false);
    int64_t y_iters = numIters(vec, false, true, false);
    int64_t z_iters = numIters(vec, false, false, true);

    return lcm(lcm(x_iters, y_iters), z_iters);
}

int main() {
    ifstream in("../input/day_12.txt");
    string buf;

    vector<Moon> vec;

    while (getline(in, buf)) {
        int64_t x = stoi(buf.substr(buf.find('=')+1, buf.find(',') - buf.find('=') - 1));
        int64_t ystart = buf.find('y');
        int64_t y = stoi(buf.substr(buf.find('=', ystart)+1, buf.find(',', ystart) - buf.find('=', ystart) - 1));
        int64_t zstart = buf.find('z');
        int64_t z = stoi(buf.substr(buf.find('=', zstart)+1, buf.find(',', zstart) - buf.find('=', zstart) - 1));
        vec.emplace_back(x, y, z);
    }

    cout << part_1(vec) << endl;
    cout << part_2(vec) << endl;

    return 0;
}
