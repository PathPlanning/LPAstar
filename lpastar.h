#ifndef LPASTAR_H
#define LPASTAR_H

#include "map.h"
#include "openlist.h"
#include "heuristics.h"

class LPAstar
{
public:
    LPAstar();
    ~LPAstar(void);

    void Initialize(const Map &map);
    void UpdateVertex(Node *vertex, const Map &map);
    bool ComputeShortestPath( const Map &map);
    double GetCost(Cell from, Cell to) const;
    std::vector<double> CalculateKey(Node vertex, const Map &map);

    std::vector<Node*> GetSuccessors(Node *curr, const Map &map);
    std::vector<Node> FindNeighbors(Node *curr, const Map &map) const;
    Result FindThePath(const Map &map, Algorithm alg);
    void MakePrimaryPath(Node curNode);

private:
    Node *start;
    Node *goal;

    double linecost;
    Result current_result;
    std::list<Node> path;
    OpenList OPEN;
    std::unordered_map<int, Node> NODES;
};

#endif // LPASTAR_H
