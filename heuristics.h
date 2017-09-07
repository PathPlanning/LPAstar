#ifndef HEURISTICS
#define HEURISTICS

#include "node.h"
#include "gl.const.h"
#include <cmath>

inline double manhattan_heuristic(Cell a, Cell b) {
  return abs(a.x - b.x) + abs(a.y - b.y);
}

inline double euclid_heuristic(Cell a, Cell b) {
    return sqrt(abs(a.x - b.x) * abs(a.x - b.x) + abs(a.y - b.y) * abs(a.y - b.y));
}

inline double octile_heuristic(Cell a, Cell b) {
    return (abs(abs(a.x - b.x) - abs(a.y - b.y)) + sqrt(2) * (std::min(abs(a.x - b.x), abs(a.y - b.y))));
}

inline double cheb_heuristic(Cell a, Cell b) {
    return std::max(abs(a.x - b.x), abs(a.y - b.y));
}


template <typename Graph>
inline double heuristic(Cell a, Cell b, const Graph &graph) {
    std::string h = graph.algorithm_info.metrictype;
    if (h.find(CNS_EUCLID) != std::string::npos) return euclid_heuristic(a, b);
    if (h.find(CNS_MANHATTAN) != std::string::npos) return manhattan_heuristic(a, b);
    if (h.find(CNS_OCTILE) != std::string::npos) return octile_heuristic(a, b);
    if (h.find(CNS_CHEB) != std::string::npos) return cheb_heuristic(a, b);
    return 1;
}

#endif // HEURISTICS

