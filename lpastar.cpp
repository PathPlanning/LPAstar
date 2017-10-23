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
    if (to->point.x == from->point.x) {
        return (to->parent->point == Cell(from->point.x - 1, from->point.y) ||
                to->parent->point == Cell(from->point.x + 1, from->point.y));
    }
    else if (to->point.y == from->point.y) {
        return (to->parent->point == Cell(from->point.x, from->point.y - 1) ||
                to->parent->point == Cell(from->point.x, from->point.y + 1));
    }
    else return false;
}

double LPAstar::GetCost(Cell from, Cell to, Map &map) const {
    if (from.x == to.x || from.y == to.y) return 1;
    else return CN_SQRT_TWO;
}

Key LPAstar::CalculateKey(const Node& vertex, Map &map) {
    Key res(std::min(vertex.g, vertex.rhs + hweight * heuristic(vertex.point, map.goal, map)), std::min(vertex.g, vertex.rhs));
    return res;
}

//The main pathbuilding function
LPASearchResult LPAstar::FindThePath(Map &map, EnvironmentOptions options, std::vector<Changes> changes)
{
    opt = options;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    number_of_steps = 0;
    Initialize(map); //algorithm initialization

    if(!ComputeShortestPath(map)) {
        current_result.pathfound = false;
        current_result.pathlength = 0;
        end = std::chrono::system_clock::now();
        current_result.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1000000000;
        std::cout << "Pathbuilding algoritm's error: THE PATH DOES NOT EXISTS\n";
        return current_result;
    }
    for (auto iteration : changes) {
        map.ChangeMap(iteration);
        for (auto dam : iteration.occupied) {
            if (NODES.count(vertex(dam, map.height))) {
                Node* d = &(NODES.find(vertex(dam, map.height))->second);
                OPEN.remove_if(d);
                for (auto neighbor: GetSurroundings(d, map)) {
                    if (!(neighbor->point == map.start) && (neighbor->parent->point == dam || CutOrSqueeze(neighbor, d))) {
                        Node min_val = GetMinPredecessor(neighbor, map);
                        if (!min_val.parent) {
                            OPEN.remove_if(neighbor);
                            if(neighbor->point == goal->point) { //means that after changes goal point became unreachable
                                current_result.pathfound = false;
                                current_result.pathlength = 0;
                                end = std::chrono::system_clock::now();
                                current_result.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1000000000;
                                std::cout << "Pathbuilding algoritm's error: AFTER CHANGES THE PATH DOES NOT EXISTS\n";
                                return current_result;
                            }
                        } else {
                            neighbor->rhs = min_val.rhs;
                            neighbor->parent = min_val.parent;
                            UpdateVertex(neighbor, map);
                        }
                    }
                }
            }
        }
        for (auto cleared : iteration.cleared) { //for each cell, that was cleared (1 -> 0) recounting values for it & it's neighbors
           Node new_node(cleared);
           new_node.rhs = std::numeric_limits<double>::infinity();
           new_node.g = std::numeric_limits<double>::infinity();
           NODES[vertex(cleared, map.height)] = new_node;
           Node * cl = &(NODES.find(vertex(cleared, map.height))->second);
           Node min_val = GetMinPredecessor(cl, map);
           if (min_val.parent) {
               cl->rhs = min_val.rhs;
               cl->parent = min_val.parent;
               cl->g = min_val.parent->g + GetCost(cl->point, min_val.parent->point, map);
               UpdateVertex(cl, map);
           } else {
               break; //means that this cell is unreachable and there's no need to recount values for it's neighbors
           }
           for (auto neighbor : GetSuccessors(cl, map)) {
               if (neighbor->rhs > cl->g + GetCost(neighbor->point, cl->point, map)) {
                   neighbor->parent = cl;
                   neighbor->rhs = cl->g + GetCost(neighbor->point, cl->point, map);
                   UpdateVertex(neighbor, map);
               }
               if (neighbor->point.x == cl->point.x || neighbor->point.y == cl->point.y) {
                   Node min_val = GetMinPredecessor(neighbor, map);
                   if (!min_val.parent) {
                       OPEN.remove_if(neighbor);
                       if(neighbor->point == goal->point) { //means that goal became unreachable
                           current_result.pathfound = false;
                           current_result.pathlength = 0;
                           end = std::chrono::system_clock::now();
                           current_result.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1000000000;
                           std::cout << "Pathbuilding algoritm's error: AFTER CHANGES THE PATH DOES NOT EXISTS\n";
                           return current_result;
                       }
                   } else {
                       neighbor->rhs = min_val.rhs;
                       neighbor->parent = min_val.parent;
                       UpdateVertex(neighbor, map);
                   }
               }
           }
        }
    }
    if(!ComputeShortestPath(map)) {
        current_result.pathfound = false;
        current_result.pathlength = 0;
        end = std::chrono::system_clock::now();
        current_result.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1000000000;
        std::cout << "Pathbuilding algoritm's error: AFTER CHANGES THE PATH DOES NOT EXISTS\n";
        return current_result;
    }
    end = std::chrono::system_clock::now();
    current_result.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1000000000;
    if (current_result.pathfound) {
        makeSecondaryPath();
        current_result.hppath = &hpath;
    }
    return current_result;
}



void LPAstar::Initialize(Map &map)
{
    Cell buf = map.start;
    map.start = map.goal;
    map.goal = buf;
    Node start_node = Node(map.start);
    Node goal_node = Node(map.goal);
    start_node.rhs = 0;
    start_node.g = std::numeric_limits<double>::infinity();
    start_node.key = CalculateKey(start_node, map);
    NODES[vertex(map.start, map.height)] = start_node;
    start = &(NODES.find(vertex(map.start, map.height))->second);

    goal_node.g = std::numeric_limits<double>::infinity();
    goal_node.rhs = std::numeric_limits<double>::infinity();

    NODES[vertex(map.goal, map.height)] = goal_node;
    goal = &(NODES.find(vertex(map.goal, map.height))->second);

    OPEN.resize(map.height);
    OPEN.put(start); //add start cell to OPEN list
}

void LPAstar::UpdateVertex(Node* u, Map &map) //adding and removing vertices from OPEN list
{
    if (!(u->IsConsistent())) {
        u->key = CalculateKey(*u, map);
        OPEN.put(u); //add vertex u in OPEN list or change it's key if it is already there
    } else {
        OPEN.remove_if(u); //rif vertex u is in OPEN list, remove it
    }
}

//the main process of opening vertices and recalculating g- and rhs-values
bool LPAstar::ComputeShortestPath(Map &map)
{
    while (OPEN.top_key_less_than(CalculateKey(*goal, map)) || goal->rhs != goal->g) {
        ++number_of_steps;
        Node* current = OPEN.get(); //returns element from OPEN with smalest key value
        if (current->g > current->rhs) {
            current->g = current->rhs;
            for (auto elem : GetSuccessors(current, map)) { //for each successor(neighbor) recalculate it's rhs value
                if (elem->rhs > current->g + GetCost(elem->point, current->point, map)) {
                    elem->parent = current;
                    elem->rhs = current->g + GetCost(elem->point, current->point, map); 
                }
                UpdateVertex(elem, map);
            }
        } else {
            current->g = std::numeric_limits<double>::infinity();
            std::vector<Node* > succ = GetSuccessors(current, map);
            succ.push_back(current);
            for (auto elem : succ) {
                if (!(elem->point == map.start) && elem->parent->point == current->point) {
                    Node min_val = GetMinPredecessor(elem, map);
                    elem->rhs = min_val.rhs;
                    elem->parent = min_val.parent;
                }
                UpdateVertex(elem, map);
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
        //map.PrintPath(path); //can use this function to build path in console
        return true;
    }
    return false;
}

//function returns the best(minimum) cost of getting to the current cell from it's predecessors and pointer to that predecessor
Node LPAstar::GetMinPredecessor(Node* current, Map &map) {
    Node* min;
    std::vector<Node *> all_neighbors;
    for(auto elem : FindNeighbors(current, map)) {
        if(!NODES.count(vertex(elem.point, map.height))) { //if vertex wasn't previously examined
            elem.g =  std::numeric_limits<double>::infinity();
            elem.rhs = std::numeric_limits<double>::infinity();
            NODES[vertex(elem.point, map.height)] = elem;
            all_neighbors.push_back(&(NODES.find(vertex(elem.point, map.height))->second));
        } else {
            all_neighbors.push_back(&(NODES.find(vertex(elem.point, map.height))->second));
        }
    }
    Node min_node;
    if (!all_neighbors.empty()) {
        min = (all_neighbors.front());
        min_node = *min;
        min_node.rhs = std::numeric_limits<double>::infinity();
        min_node.parent = min;
        for (auto n: all_neighbors) {
            if (min_node.rhs > n->g + GetCost(n->point, current->point, map)) {
                min_node.rhs = n->g + GetCost(n->point, current->point, map);
                min_node.parent = n;
            }
        }
    } else {
        min_node.parent = nullptr; //means that current vertex is now unreachable
    }
    return min_node;
}

//function returns Nodes of successors of current vertex. Already with their g- and rhs-values
std::vector<Node* > LPAstar::GetSuccessors(Node* current, Map &map) {
    std::vector<Node*> result;
    for(auto elem : FindNeighbors(current, map)) {
        if(!NODES.count(vertex(elem.point, map.height))) { //if vertex wasn't previously examined
            elem.g =  std::numeric_limits<double>::infinity();
            elem.rhs = std::numeric_limits<double>::infinity();
            NODES[vertex(elem.point, map.height)] = elem;
            result.push_back(&(NODES.find(vertex(elem.point, map.height))->second));
        } else {
            result.push_back(&(NODES.find(vertex(elem.point, map.height))->second));
        }
    }
    return result;
}

//function returns list of map neighbors to the current node depending on the environmental conditions
std::list<Node> LPAstar::FindNeighbors(Node* n, Map &map) const {
    Node newNode;
    Cell curNode = n->point;
    std::list<Node> successors;
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
                newNode = Node(Cell(curNode.x + j, curNode.y + i), n);
                successors.push_front(newNode);
            }
        }
    }
    return successors;
}

//function returns surroundings - neighbors of the current cell, but not depending on envinonmental options (cutcorners, allowsqueeze)
std::list<Node*> LPAstar::GetSurroundings(Node *current, Map &map) {
    std::list<Node> result1;
    int x = current->point.x;
    int y = current->point.y;
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
        if(!NODES.count(vertex(elem.point, map.height))) { //if vertex wasn't previously examined
            continue;
        } else {
            result.push_back(&(NODES.find(vertex(elem.point, map.height))->second));
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
        curI = iter->point.y;
        curJ = iter->point.x;
        ++iter;
        nextI = iter->point.y;
        nextJ = iter->point.x;
        moveI = nextI - curI;
        moveJ = nextJ - curJ;
        ++iter;
        if ((iter->point.y - nextI) != moveI || (iter->point.x - nextJ) != moveJ)
            hpath.push_back(*(--iter));
        else
            --iter;
    }
}
