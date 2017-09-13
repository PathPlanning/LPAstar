#ifndef LPASTAR_H
#define LPASTAR_H

#include "map.h"
#include "openlist.h"
#include "heuristics.h"
#include <set>

class LPAstar
{
public:
    LPAstar();
    ~LPAstar(void);

    Result FindThePath(Map &map, Algorithm alg);
    void MakePrimaryPath(Node curNode);

private:
    Node *start;
    Node *goal;

    double linecost;
    Result current_result;
    std::list<Node> path;
    OpenList OPEN;
    std::unordered_map<int, Node> NODES;

    void Initialize(Map &map);
    void UpdateVertex(Node* u, Map &map);
    bool ComputeShortestPath(Map &map);
    void CloseOpen(double height);
    double GetCost(Cell from, Cell to, Map &map) const;
    Key CalculateKey(const Node &vertex, Map &map);

    std::vector<Node *> GetSuccessors(Node curr, Map &map);
    std::vector<Node> FindNeighbors(Node curr, Map &map) const;
};

#endif // LPASTAR_H
