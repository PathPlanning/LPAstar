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

struct Key{
    double k1, k2;

    Key() : k1(std::numeric_limits<double>::infinity()), k2(std::numeric_limits<double>::infinity()) {}
    Key(double k1_, double k2_) : k1(k1_), k2(k2_) {}
    Key(const Key& other) : k1(other.k1), k2(other.k2) {}

    inline bool operator==(const Key& p) const {
        return k1 == p.k1 && k2 == p.k2;
    }
    inline bool operator<(const Key& p) const {
        return k1 < p.k1 || (k1 == p.k1 && k2 <= p.k2);
    }

    Key& operator=(const Key& other) {
        if (this == &other) {
            return *this;
        }
        k1 = other.k1;
        k2 = other.k2;
        return *this;
    }

};


class Node {
public:
    double rhs, g, h;
    Key key;
    Cell point;
    Node *parent;

    Node() {}
    Node(const Cell& p, Node *c = nullptr) : g(std::numeric_limits<double>::infinity()),
        rhs(std::numeric_limits<double>::infinity()), point(p), parent(c) {}

    bool operator==(const Node& another) const {
        return point == another.point && rhs == another.rhs
                &&parent == another.parent && g == another.g && h == another.h;
    }

    Node& operator=(const Node& other) {
        point = other.point;
        rhs = other.rhs;
        g = other.g;
        parent = other.parent;
        key = other.key;
        return *this;
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

struct ANode
{
    int     i, j;
    double  F, g, H;
    ANode    *parent;

    bool operator== (const ANode &other) const {
        return i == other.i && j == other.j;
    }
};


#endif // NODE_H
