#ifndef STRUCTURES
#define STRUCTURES

#include "node.h"
#include <cmath>
#include <vector>
#include <functional>
#include <utility>
#include <iostream>
#include <chrono>

struct Changes {
    std::list<Cell> occupied;
    std::list<Cell> cleared;
};

struct Section {
    Node start;
    Node finish;
    double length;

    Section(Node s, Node f) : start(s), finish(f), length(sqrt(pow(f.point.x - s.point.x, 2) + pow(f.point.y - s.point.y, 2))) {}
};

struct Figure {
    double perimeter;
    double square;
    std::vector<Cell> cells;
    Figure() {}
    Figure(int p = 0, int s = 0, std::vector<Cell> c = {}) : perimeter(p), square(s), cells(c) {}
};

struct Algorithm {
    std::string searchtype;
    std::string metrictype;
    double hweight;
    std::string breakingties;
    double linecost;
    double diagonalcost;
    bool allowdiagonal;
    bool allowsqueeze;
    bool cutcorners;
    double loglevel;
    std::string logpath;
    std::string logfilename;
};

struct Path {
    std::vector<Node> path_lp;
    std::vector<Section> path_hp;
};

struct Result {
    bool path_existance;
    Path path;
    int steps;
    size_t nodes;
    double length;
    double time;

    Result() {}
    Result(bool p, int s, int n, double l) : path_existance(p), steps(s), nodes(n), length(l) {}
    void put(bool p, size_t n, double l, std::chrono::time_point<std::chrono::system_clock> start, std::chrono::time_point<std::chrono::system_clock> end) {
        path_existance = p;
        nodes = n;
        length = l;
        time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1000000000;
    }
};


#endif // STRUCTURES

