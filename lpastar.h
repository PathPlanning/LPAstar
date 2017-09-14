#ifndef LPASTAR_H
#define LPASTAR_H

#include "map.h"
#include "openlist.h"
#include "searchresult.h"
#include "heuristics.h"
#include "environmentoptions.h"
#include <unordered_map>
#include <set>
#include <chrono>

class LPAstar
{
public:
    LPAstar();
    ~LPAstar(void);

    LPASearchResult FindThePath(Map &map, EnvironmentOptions options);
    void MakePrimaryPath(Node* curNode);
    void makeSecondaryPath();

private:
    Node *start;
    Node *goal;
    int number_of_steps;

    EnvironmentOptions opt;
    std::list<Node> path;
    std::list<Node> hpath;

    double linecost;
    LPASearchResult current_result;
    OpenList OPEN;
    std::unordered_map<int, Node> NODES;

    void Initialize(Map &map);
    void UpdateVertex(Node* u, Map &map);
    bool ComputeShortestPath(Map &map);
    void CloseOpen(double height);
    double GetCost(Cell from, Cell to, Map &map) const;
    Key CalculateKey(const Node &vertex, Map &map);

    std::vector<Node *> GetSuccessors(Node *curr, Map &map);
    std::list<Node *> GetSurroundings(Node *current, Map &map);
    Node GetMinPredecessor(Node* curr, Map &map);
    std::list<Node> FindNeighbors(Node* curr, Map &map) const;
};

#endif // LPASTAR_H
