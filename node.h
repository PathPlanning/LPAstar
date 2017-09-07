#ifndef NODE_H
#define NODE_H

#include <memory>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <map>
#include <cmath>
#include <iostream>

struct Cell {
    int x, y;

    Cell() {}
    Cell(int x_, int y_) : x(x_), y(y_) {}
    Cell(Cell other, int x_, int y_) : x(other.x + x_), y(other.y + y_) {}

    inline bool operator==(const Cell& p) const {
        return x == p.x && y == p.y;
    }
    inline bool operator<(const Cell& p) const {
        return y < p.y || (y == p.y && x < p.x);
    }
    Cell& operator=(const Cell& other) {
        if (this == &other) {
            return *this;
        }
        x = other.x;
        y = other.y;
        return *this;
    }
        Cell& operator+=(const Cell& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
};
inline Cell operator+(const Cell& one, const Cell& other) {
    Cell newc;
    newc.x = one.x + other.x;
    newc.y = one.y + other.y;
    return newc;
}

inline std::ostream& operator<< (std::ostream& out, const Cell &next) {
    out << "(" << next.x << "," << next.y << "); ";
    return out;
}


class Node {
public:
    Node() {}
    Node(const Cell& p, Node *c = nullptr) : g(0), point(p), parent(c) {}
    double rhs, g, h;
    std::vector<double> key;
    Cell point;
    Node *parent;


    inline void clear_state() { rhs = std::numeric_limits<double>::max();
                                g = std::numeric_limits<double>::max(); parent = nullptr; }

    bool operator==(const Node& another) const {
        return point == another.point && rhs == another.rhs
                &&parent == another.parent && g == another.g && h == another.h;
    }

    bool IsConsistent() const {
        return g == rhs;
    }
};

inline bool operator<(const Node& one, const Node& another) {
    return one.point.x < another.point.x;
}

inline std::ostream& operator<< (std::ostream& out, const Node &next) {
    out << "(" << next.point.x << "," << next.point.y << "); ";
    return out;
}


#endif // NODE_H
