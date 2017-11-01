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
    LPAstar(double HW);
    ~LPAstar(void);

    //main function for the whole pathbuilding algorithm
    LPASearchResult FindThePath(Map &map, EnvironmentOptions options);

private:
    Node *start;
    Node *goal;
    int number_of_steps;
    double hweight;

    //EnvironmentOptions opt;
    std::list<Node> path;
    std::list<Node> hpath;

    LPASearchResult current_result;
    OpenList OPEN;
    std::unordered_map<int, Node> NODES;

    void Initialize(Map &map, int metrics);
    void UpdateVertex(Node* u, int metrics);
    bool ComputeShortestPath(Map &map, EnvironmentOptions opt);
    double GetCost(Cell from, Cell to) const;
    Key CalculateKey(Node &vertex, int metrics);
    std::vector<Node *> GetSuccessors(Node *curr, Map &map, EnvironmentOptions opt);
    std::list<Node *> GetSurroundings(Node *current, Map &map, EnvironmentOptions opt);
    Node GetMinPredecessor(Node* curr, Map &map, EnvironmentOptions opt);
    std::list<Cell> FindNeighbors(Node* curr, Map &map, EnvironmentOptions opt) const;

    //functions for path building
    void MakePrimaryPath(Node* curNode);
    void makeSecondaryPath();
};

#endif // LPASTAR_H
