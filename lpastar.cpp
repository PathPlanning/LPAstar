#include "lpastar.h"


inline int vertex(Cell item, int size) {
    return item.x * size + item.y;
}

double LPAstar::GetCost(Cell from, Cell to, Map &map) const {
    return linecost * euclid_heuristic(from, to);
}

Key LPAstar::CalculateKey(const Node& vertex, Map &map) {
    Key res(std::min(vertex.g, vertex.rhs + heuristic(vertex.point, map.goal, map)), std::min(vertex.g, vertex.rhs));
    return res;
}

LPAstar::LPAstar() { OPEN.set_size(0); }
LPAstar::~LPAstar()
{
    if (!NODES.empty()) NODES.erase(NODES.begin(), NODES.end());
}

Result LPAstar::FindThePath(Map &map, Algorithm alg)
{

    linecost = alg.linecost;
    Initialize(map);

    if(!ComputeShortestPath(map))
        std::cout << "OOOPS\n";
    else
        std::cout << "Done\n";
    std::cout << current_result.length <<std::endl;
    for (auto elem : path) {
        std::cout << elem.point;
    }
    std::cout << std::endl;
    map.DamageTheMap(path);
    std::vector<Node> damaged = {Node(Cell(1,4)), Node(Cell(2, 4)), Node(Cell(0,4)), Node(Cell(3,4))};
    for (auto dam : damaged) {
        for (auto neighbor: GetSuccessors(dam, map)) {
            if (!(neighbor->point == map.start) && neighbor->parent->point == dam.point) {
                Node min_val;
                min_val.rhs = std::numeric_limits<double>::infinity();
                for (auto pred : GetSuccessors(*neighbor, map)) {
                    if (min_val.rhs > pred->g + GetCost(neighbor->point, pred->point, map)) {
                        min_val.parent = pred;
                        min_val.rhs = pred->g + GetCost(neighbor->point, pred->point, map);
                    }
                }
                neighbor->rhs = min_val.rhs;
                neighbor->parent = min_val.parent;
                std::cout << "changed: "
                          << neighbor->point << ' ' << neighbor->parent->point << std::endl;
                UpdateVertex(neighbor, map);
            }
        }
    }
    if(ComputeShortestPath(map)){
        std::cout << "ALL OK\n";
    } else
        std::cout << "NOT OK\n";
    for (auto elem: path) std::cout << elem.point << " ";
    std::cout << std::endl;
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

    OPEN.resize(map.width);
    OPEN.put(start);
    current_result = Result(false, 0, 0, 0);
}

void LPAstar::CloseOpen(double height) {
    for (auto elem : OPEN.get_elements()) {
        NODES[vertex(elem.point, height)] = elem;
    }
}

void LPAstar::UpdateVertex(Node* u, Map &map)
{
    if (!u->IsConsistent()) {
        u->key = CalculateKey(*u, map);
        OPEN.put(u); //check if it is already there
    } else {
        OPEN.remove_if(u);
    }
}

bool LPAstar::ComputeShortestPath(Map &map)
{
    while (OPEN.top_key_less_than(CalculateKey(*goal, map)) || goal->rhs != goal->g) {
        Node* current = OPEN.get();
        if (current->g > current->rhs) {
            current->g = current->rhs;
            OPEN.pop();
            for (auto elem : GetSuccessors(*current, map)) {
                if (elem->rhs > current->g + GetCost(elem->point, current->point, map)) {
                    elem->parent = current;
                    elem->rhs = current->g + GetCost(elem->point, current->point, map);
                    UpdateVertex(elem, map);
                }
            }
        } else {
            current->g = std::numeric_limits<double>::infinity();
            std::vector<Node* > succ = GetSuccessors(*current, map);
            succ.push_back(current);
            for (auto elem : succ) {
                if (!(elem->point == map.start) && elem->parent->point == current->point) {
                    Node min_val;
                    min_val.rhs = std::numeric_limits<double>::infinity();
                    for (auto pred : GetSuccessors(*elem, map)) {
                        if (pred->g + GetCost(pred->point, elem->point, map) < min_val.rhs) {
                            min_val.rhs = pred->g + GetCost(pred->point, elem->point, map);
                            min_val.point = pred->point;
                            min_val.parent = pred;
                        }
                    }
                    elem->rhs = min_val.rhs;
                    elem->parent = min_val.parent;
                    UpdateVertex(elem, map);
                }
            }
        }
        if(current->parent) {
            std::cout << current->point << "g " << current->g << " rhs" << current->rhs <<
                  current->parent->point << std::endl;
        }

    }
    if (goal->g != std::numeric_limits<double>::infinity()) {
        current_result = Result(true, OPEN.size_of_open() + NODES.size(), 0, goal->g);
        std::cout << goal->g << std::endl;
        MakePrimaryPath(*goal);
        return true;
    }
    return false;
}


std::vector<Node* > LPAstar::GetSuccessors(Node current, Map &map) {
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

std::vector<Node> LPAstar::FindNeighbors(Node n, Map &map) const {
    std::vector<Node> result;
    int x = n.point.x;
    int y = n.point.y;
    if(map.algorithm_info.allowdiagonal) {
        for (int k = y - 1; k <= y + 1; ++k) {
            for (int l = x - 1; l <= x + 1; ++l) {
                if (!(k == y && l == x) && map.CellIsNeighbor(Cell(l, k), n.point)) {
                    result.push_back(Node(Cell(l, k)));
                }
            }
        }
    } else {
        for (int k = x - 1; k <= x + 1; ++k)
            if (k != x && map.CellOnGrid(Cell(k, y)) && map.CellIsTraversable(Cell(k, y)))
                result.push_back(Node(Cell(k, y)));
        for (int l = y - 1; l <= y + 1; ++l)
            if (l != y && map.CellOnGrid(Cell(x, l)) && map.CellIsTraversable(Cell(x, l)))
                result.push_back(Node(Cell(x, l)));
    }
    return result;
}

void LPAstar::MakePrimaryPath(Node curNode)
{
    path.clear();
    Node current = curNode;
    while (current.parent) {
        path.push_front(current);
        current = *current.parent;
    }
    path.push_front(current);
}
