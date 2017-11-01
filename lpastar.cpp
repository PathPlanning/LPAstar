#include "lpastar.h"

LPAstar::LPAstar() {}
LPAstar::LPAstar(double HW) { hweight = HW; }
LPAstar::~LPAstar()
{
    if (!NODES.empty()) NODES.erase(NODES.begin(), NODES.end());
}

inline int vertex(Cell item, int size) {
    return item.x * size + item.y;
}

inline bool CutOrSqueeze(Node* to, Node* from) {
    if (to->cell.x == from->cell.x) {
        return (to->parent->cell == Cell(from->cell.x - 1, from->cell.y) ||
                to->parent->cell == Cell(from->cell.x + 1, from->cell.y));
    }
    else if (to->cell.y == from->cell.y) {
        return (to->parent->cell == Cell(from->cell.x, from->cell.y - 1) ||
                to->parent->cell == Cell(from->cell.x, from->cell.y + 1));
    }
    else return false;
}

double LPAstar::GetCost(Cell from, Cell to) const {
    if (from.x == to.x || from.y == to.y) return 1;
    else return CN_SQRT_TWO;
}

Key LPAstar::CalculateKey(Node& vertex, int metrics) {
    Key res(std::min(vertex.g, vertex.rhs + hweight * heuristic(vertex.cell, goal->cell, metrics)), std::min(vertex.g, vertex.rhs));
    return res;
}

//The main pathbuilding function
LPASearchResult LPAstar::FindThePath(Map &map, EnvironmentOptions options)
{
    std::chrono::time_point<std::chrono::system_clock> startt, end;
    startt = std::chrono::system_clock::now();
    number_of_steps = 0;
    Initialize(map, options.metrictype); //algorithm initialization

    if(!ComputeShortestPath(map, options)) {
        current_result.pathfound = false;
        current_result.pathlength = 0;
        end = std::chrono::system_clock::now();
        current_result.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - startt).count()) / 1000000000;
        std::cout << "THE PATH DOES NOT EXIST ON THE INITIAL MAP\n";
        return current_result;
    }
    Changes changes = map.DamageTheMap(path); //force map to change (sufficient for the correct testing)
    for (auto dam : changes.occupied) { //for each damaged (0 -> 1) cell recounting values for it's neighbors
        if (NODES.count(vertex(dam, map.get_height()))) {
            Node* d = &(NODES.find(vertex(dam, map.get_height()))->second);
            OPEN.remove_if(d);
            for (auto neighbor: GetSurroundings(d, map, options)) {
                if (!(neighbor->cell == start->cell) && (neighbor->parent->cell == dam || CutOrSqueeze(neighbor, d))) {
                    Node min_val = GetMinPredecessor(neighbor, map, options);
                    if (!min_val.parent) { //means that neighbor is now unreachable
                        OPEN.remove_if(neighbor);
                        if(neighbor->cell == goal->cell) { //means that after changes goal cell became unreachable
                            current_result.pathfound = false;
                            current_result.pathlength = 0;
                            end = std::chrono::system_clock::now();
                            current_result.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - startt).count()) / 1000000000;
                            std::cout << "AFTER THE FIRST MAP CHANGE THE GOAL CELL BECAME UNREACHABLE. PATH DOES NOT EXIST\n";
                            current_result.goal_became_unreachable = true;
                            return current_result;
                        }
                    } else {
                        neighbor->rhs = min_val.rhs;
                        neighbor->parent = min_val.parent;
                        UpdateVertex(neighbor, options.metrictype);
                    }
                }
            }
        }
    }
    if(!ComputeShortestPath(map, options)) {
        current_result.pathfound = false;
        current_result.pathlength = 0;
        end = std::chrono::system_clock::now();
        current_result.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - startt).count()) / 1000000000;
        std::cout << "AFTER THE FIRST MAP CHANGE THE PATH DOES NOT EXIST\n";
        return current_result;
    }
    changes = map.ClearTheMap(changes.occupied); //force map to change (here: clear all previous damage)
    for (auto cleared : changes.cleared) { //for each cell, that was cleared (1 -> 0) recounting values for it & it's neighbors
       Node new_node(cleared);
       new_node.rhs = std::numeric_limits<double>::infinity();
       new_node.g = std::numeric_limits<double>::infinity();
       NODES[vertex(cleared, map.get_height())] = new_node;
       Node * cl = &(NODES.find(vertex(cleared, map.get_height()))->second);
       Node min_val = GetMinPredecessor(cl, map, options);
       if (min_val.parent) { //means that neighbor is reachable
           cl->rhs = min_val.rhs;
           cl->parent = min_val.parent;
           cl->g = min_val.parent->g + GetCost(cl->cell, min_val.parent->cell);
           UpdateVertex(cl, options.metrictype);
       } else {
           break; //means that this cell is unreachable and there's no need to recount values for it's neighbors
       }
       for (auto neighbor : GetSuccessors(cl, map, options)) {
           if (neighbor->rhs > cl->g + GetCost(neighbor->cell, cl->cell)) {
               neighbor->parent = cl;
               neighbor->rhs = cl->g + GetCost(neighbor->cell, cl->cell);
               UpdateVertex(neighbor, options.metrictype);
           }
           if (neighbor->cell.x == cl->cell.x || neighbor->cell.y == cl->cell.y) {
               Node min_val = GetMinPredecessor(neighbor, map, options);
               if (!min_val.parent) { //means that neighbor is now unreachable
                   OPEN.remove_if(neighbor);
                   if(neighbor->cell == goal->cell) { //means that goal became unreachable
                       current_result.pathfound = false;
                       current_result.pathlength = 0;
                       end = std::chrono::system_clock::now();
                       current_result.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - startt).count()) / 1000000000;
                       std::cout << "AFTER THE SECOND MAP CHANGE THE GOAL CELL BECAME UNREACHABLE. PATH DOES NOT EXIST\n";
                       current_result.goal_became_unreachable = true;
                       return current_result;
                   }
               } else {
                   neighbor->rhs = min_val.rhs;
                   neighbor->parent = min_val.parent;
                   UpdateVertex(neighbor, options.metrictype);
               }
           }
       }
    }
    if(!ComputeShortestPath(map, options)){
        current_result.pathfound = false;
        current_result.pathlength = 0;
        end = std::chrono::system_clock::now();
        current_result.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - startt).count()) / 1000000000;
        std::cout << "AFTER THE SECOND MAP CHANGE THE PATH DOES NOT EXIST\n";
        return current_result;
    }
    end = std::chrono::system_clock::now();
    current_result.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - startt).count()) / 1000000000;
    if (current_result.pathfound) {
        makeSecondaryPath();
        current_result.hppath = &hpath;
    }
    return current_result;
}



void LPAstar::Initialize(Map &map, int metrics)
{
    Node start_node = Node(map.get_start());
    Node goal_node = Node(map.get_goal());
    goal_node.g = std::numeric_limits<double>::infinity();
    goal_node.rhs = std::numeric_limits<double>::infinity();

    NODES[vertex(map.get_goal(), map.get_height())] = goal_node;
    goal = &(NODES.find(vertex(map.get_goal(), map.get_height()))->second);

    start_node.rhs = 0;
    start_node.g = std::numeric_limits<double>::infinity();
    start_node.key = CalculateKey(start_node, metrics);
    NODES[vertex(map.get_start(), map.get_height())] = start_node;
    start = &(NODES.find(vertex(map.get_start(), map.get_height()))->second);


    OPEN.resize(map.get_height());
    OPEN.put(start); //add start cell to OPEN list

    current_result.goal_became_unreachable = false;
}

void LPAstar::UpdateVertex(Node* u, int metrics) //adding and removing vertices from OPEN list
{
    if (!(u->IsConsistent())) {
        u->key = CalculateKey(*u, metrics);
        OPEN.put(u); //add vertex u in OPEN list or change it's key if it is already there
    } else {
        OPEN.remove_if(u); //if vertex u is in OPEN list, remove it
    }
}

//the main process of opening vertices and recalculating g- and rhs-values
bool LPAstar::ComputeShortestPath(Map &map, EnvironmentOptions opt)
{
    while (OPEN.top_key_less_than(CalculateKey(*goal, opt.metrictype)) || goal->rhs != goal->g) {
        ++number_of_steps;
        Node* current = OPEN.get(); //returns element from OPEN with smalest key value
        if (current->g > current->rhs) {
            current->g = current->rhs;
            for (auto elem : GetSuccessors(current, map, opt)) { //for each successor(neighbor) recalculate it's rhs value
                if (elem->rhs > current->g + GetCost(elem->cell, current->cell)) {
                    elem->parent = current;
                    elem->rhs = current->g + GetCost(elem->cell, current->cell);
                }
                UpdateVertex(elem, opt.metrictype);
            }
        } else {
            current->g = std::numeric_limits<double>::infinity();
            std::vector<Node* > succ = GetSuccessors(current, map, opt);
            succ.push_back(current);
            for (auto elem : succ) {
                if (!(elem->cell == start->cell) && elem->parent->cell == current->cell) {
                    Node min_val = GetMinPredecessor(elem, map, opt);
                    elem->rhs = min_val.rhs;
                    elem->parent = min_val.parent;
                }
                UpdateVertex(elem, opt.metrictype);
            }
        }
    }
    if (goal->g != std::numeric_limits<double>::infinity()) { //if path exists
        current_result.pathfound = true;
        current_result.numberofsteps = number_of_steps;
        current_result.nodescreated = NODES.size();
        current_result.pathlength = goal->g;
        MakePrimaryPath(goal); //build path from parent pointers
        current_result.lppath = &path;
        map.PrintPath(path); //can use this function to build path in console
        return true;
    }
    return false;
}

//function returns "dummy" Node for current Node, that consists of parent - predecessor with the best(minimum)
//cost of getting from it to the current and rhs - this best(minimum) cost.
//when current node has no predecessors function returns parent=nullptr - which means that current vertex is now unreachable
Node LPAstar::GetMinPredecessor(Node* current, Map &map, EnvironmentOptions opt) {
    std::vector<Node *> all_neighbors = GetSuccessors(current, map, opt);
    Node dummy_current; //"dummy" for current, we will use only parent and rhs
    Node* neighbor; //neighbour of current node, we will be looking for neighbour with minimal cost from it to current
    if (!all_neighbors.empty()) {
        neighbor = all_neighbors.front();
        dummy_current.rhs = std::numeric_limits<double>::infinity(); //setting parameters for "dummy"
        dummy_current.parent = neighbor; //setting parameters for "dummy"
        for (auto n: all_neighbors) {
            if (dummy_current.rhs > n->g + GetCost(n->cell, current->cell)) {
                dummy_current.rhs = n->g + GetCost(n->cell, current->cell);
                dummy_current.parent = n;
            }
        }
    } else {
        dummy_current.parent = nullptr; //means that current vertex is now unreachable
    }
    return dummy_current;
}

//function returns Nodes of successors of current vertex. Already with their g- and rhs-values
std::vector<Node* > LPAstar::GetSuccessors(Node* current, Map &map, EnvironmentOptions opt) {
    std::vector<Node*> result;
    for(auto elem : FindNeighbors(current, map, opt)) {
        if(!NODES.count(vertex(elem, map.get_height()))) { //if vertex wasn't previously examined
            NODES[vertex(elem, map.get_height())] = Node(elem, current);
        }
        result.push_back(&(NODES.find(vertex(elem, map.get_height()))->second));
    }
    return result;
}

//function returns list of map neighbors to the current node depending on the environmental conditions
std::list<Cell> LPAstar::FindNeighbors(Node* n, Map &map, EnvironmentOptions opt) const {
    Cell new_cell;
    Cell curNode = n->cell;
    std::list<Cell> successors;
    for (int i = -1; i <= +1; i++) {
        for (int j = -1; j <= +1; j++) {
            if ((i != 0 || j != 0) && map.CellOnGrid(Cell(curNode.x + j, curNode.y + i)) &&
                    (map.CellIsTraversable(Cell(curNode.x + j, curNode.y + i)))) {
                if (i != 0 && j != 0) {
                    if (!opt.allowdiagonal)
                        continue;
                    else if (!opt.cutcorners) {
                        if (map.CellIsObstacle(Cell(curNode.x + j, curNode.y)) ||
                                map.CellIsObstacle(Cell(curNode.x, curNode.y + i)))
                            continue;
                    }
                    else if (!opt.allowsqueeze) {
                        if (map.CellIsObstacle(Cell( curNode.x + j, curNode.y)) &&
                                map.CellIsObstacle(Cell( curNode.x, curNode.y + i)))
                            continue;
                    }
                }
                new_cell = Cell(curNode.x + j, curNode.y + i);
                successors.push_front(new_cell);
            }
        }
    }
    return successors;
}

//function returns surroundings - neighbors of the current cell, but not depending on envinonmental options (cutcorners, allowsqueeze)
std::list<Node*> LPAstar::GetSurroundings(Node *current, Map &map, EnvironmentOptions opt) {
    std::list<Node> result1;
    int x = current->cell.x;
    int y = current->cell.y;
    if(opt.allowdiagonal) {
        for (int k = y - 1; k <= y + 1; ++k) {
            for (int l = x - 1; l <= x + 1; ++l) {
                if (!(k == y && l == x) && map.CellOnGrid(Cell(l, k)) && map.CellIsTraversable(Cell(l, k))) {
                    result1.push_front(Node(Cell(l, k)));
                }
            }
        }
    } else {
        for (int k = x - 1; k <= x + 1; ++k)
            if (k != x && map.CellOnGrid(Cell(k, y)) && map.CellIsTraversable(Cell(k, y)))
                result1.push_front(Node(Cell(k, y)));
        for (int l = y - 1; l <= y + 1; ++l)
            if (l != y && map.CellOnGrid(Cell(x, l)) && map.CellIsTraversable(Cell(x, l)))
                result1.push_front(Node(Cell(x, l)));
    }
    std::list<Node*> result;
    for(auto elem : result1) {
        if(!NODES.count(vertex(elem.cell, map.get_height()))) { //if vertex wasn't previously examined
            continue;
        } else {
            result.push_back(&(NODES.find(vertex(elem.cell, map.get_height()))->second));
        }
    }
    return result;
}

void LPAstar::MakePrimaryPath(Node *curNode) //build path by cells
{
    path.clear();
    Node current = *curNode;
    while (current.parent) {
        path.push_front(current);
        current = *current.parent;
    }
    path.push_front(current);
}

void LPAstar::makeSecondaryPath() //build path by sections
{
    std::list<Node>::const_iterator iter = path.begin();
    int curI, curJ, nextI, nextJ, moveI, moveJ;
    hpath.push_back(*iter);
    while (iter != --path.end()) {
        curI = iter->cell.y;
        curJ = iter->cell.x;
        ++iter;
        nextI = iter->cell.y;
        nextJ = iter->cell.x;
        moveI = nextI - curI;
        moveJ = nextJ - curJ;
        ++iter;
        if ((iter->cell.y - nextI) != moveI || (iter->cell.x - nextJ) != moveJ)
            hpath.push_back(*(--iter));
        else
            --iter;
    }
}
