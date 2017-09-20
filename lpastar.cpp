#include "lpastar.h"


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
    if (map.CellIsNeighbor(from, to)) {
        if (from.x == to.x || from.y == to.y) return 1;
        else return CN_SQRT_TWO;
    } else {
        return std::numeric_limits<double>::infinity();
    }
}

Key LPAstar::CalculateKey(const Node& vertex, Map &map) {
    Key res(std::min(vertex.g, vertex.rhs + heuristic(vertex.point, map.goal, map)), std::min(vertex.g, vertex.rhs));
    return res;
}

LPAstar::LPAstar() {}
LPAstar::~LPAstar()
{
    if (!NODES.empty()) NODES.erase(NODES.begin(), NODES.end());
}

LPASearchResult LPAstar::FindThePath(Map &map, EnvironmentOptions options)
{

    opt = options;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    number_of_steps = 0;
    Initialize(map);

    if(!ComputeShortestPath(map))
        std::cout << "OOOPS\n";
    else
        std::cout << "Done\n";
    std::cout << current_result.pathlength <<std::endl;
    Changes changes = map.DamageTheMap(path);
    for (auto dam : changes.occupied) {
        if (NODES.count(vertex(dam, map.height))) {
            Node* d = &(NODES.find(vertex(dam, map.height))->second);
            OPEN.remove_if(d);
            for (auto neighbor: GetSurroundings(d, map)) {
                //std::cout << "n: " << neighbor->point << std::endl;
                if (!(neighbor->point == map.start) && (neighbor->parent->point == dam || CutOrSqueeze(neighbor, d))) {
                    Node min_val = GetMinPredecessor(neighbor, map);
                    if (!min_val.parent) {
                        OPEN.remove_if(neighbor);
                        if(neighbor->point == goal->point) {
                            current_result.pathfound = false;
                            current_result.pathlength = 0;
                            return current_result;
                        }
                    } else {
                        neighbor->rhs = min_val.rhs;
                        neighbor->parent = min_val.parent;
                        //std::cout << "changed: "
                        //          << neighbor->point << ' ' << neighbor->parent->point << std::endl;
                        UpdateVertex(neighbor, map);
                    }
                }
            }
        }
    }
    if(ComputeShortestPath(map)){
        std::cout << "ALL OK\n";
    } else
        std::cout << "NOT OK\n";
    end = std::chrono::system_clock::now();
    current_result.time = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1000000000;
    if (current_result.pathfound) {
        makeSecondaryPath();
        current_result.hppath = &hpath;
    }
    //for (auto elem: path) std::cout << elem->point << " ";
    //std::cout << std::endl;
    return current_result;
}



void LPAstar::Initialize(Map &map)
{
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
    OPEN.put(start);
}

void LPAstar::CloseOpen(double height) {
    for (auto elem : OPEN.get_elements()) {
        NODES[vertex(elem.point, height)] = elem;
    }
}

void LPAstar::UpdateVertex(Node* u, Map &map)
{
    if (!(u->IsConsistent())) {
        u->key = CalculateKey(*u, map);
        //std::cout << "key: "<< u->key.k1 << " " << u->key.k2 << " " << u->point << std::endl;
        OPEN.put(u); //check if it is already there
    } else {
        OPEN.remove_if(u);
    }
}

bool LPAstar::ComputeShortestPath(Map &map)
{
    while (OPEN.top_key_less_than(CalculateKey(*goal, map)) || goal->rhs != goal->g) {
        ++number_of_steps;
        Node* current = OPEN.get();
        if (current->g > current->rhs) {
            current->g = current->rhs;
            OPEN.pop();
            for (auto elem : GetSuccessors(current, map)) {
                if (elem->rhs > current->g + GetCost(elem->point, current->point, map)) {
                    elem->parent = current;
                    elem->rhs = current->g + GetCost(elem->point, current->point, map);
                    UpdateVertex(elem, map);
                }
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
        /*if(current->parent) {
            std::cout << current->point << "g " << current->g << " rhs" << current->rhs <<
                  current->parent->point << std::endl;
        } */     //std::cout << OPEN.top_key().k1 << std::endl;
        //OPEN.print_elements();

    }
    if (goal->g != std::numeric_limits<double>::infinity()) {
        current_result.pathfound = true;
        current_result.numberofsteps = number_of_steps;
        current_result.nodescreated = NODES.size();
        current_result.pathlength = goal->g;
        std::cout << goal->g << std::endl;
        MakePrimaryPath(goal);
        current_result.lppath = &path;
        //map.PrintPath(path);
        return true;
    } else {
        current_result.pathfound = false;
        current_result.pathlength = 0;
    }
    return false;
}

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
        min_node.parent = nullptr;
    }
    return min_node;
}

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

std::list<Node> LPAstar::FindNeighbors(Node* n, Map &map) const {
    Node newNode;
    Cell curNode = n->point;
    std::list<Node> successors;
    for (int i = -1; i <= +1; i++)
        for (int j = -1; j <= +1; j++)
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
    return successors;
    /*int x = n->point.x;
    int y = n->point.y;
    if(!opt.allowdiagonal) {
        for (int k = y - 1; k <= y + 1; ++k) {
            for (int l = x - 1; l <= x + 1; ++l) {
                if (!(k == y && l == x) && map.CellIsNeighbor(Cell(l, k), n->point)) {
                    result.push_back(Node(Cell(l, k), n));
                }
            }
        }
    } else {
        for (int k = x - 1; k <= x + 1; ++k)
            if (k != x && map.CellOnGrid(Cell(k, y)) && map.CellIsTraversable(Cell(k, y)))
                result.push_back(Node(Cell(k, y), n));
        for (int l = y - 1; l <= y + 1; ++l)
            if (l != y && map.CellOnGrid(Cell(x, l)) && map.CellIsTraversable(Cell(x, l)))
                result.push_back(Node(Cell(x, l), n));
    }
    return result;*/
}

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

void LPAstar::MakePrimaryPath(Node *curNode)
{
    path.clear();
    Node current = *curNode;
    while (current.parent) {
        path.push_front(current);
        current = *current.parent;
    }
    path.push_front(current);
}

void LPAstar::makeSecondaryPath()
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
