#include "lpastar.h"


inline int vertex(Cell item, int size) {
    return item.x * size + item.y;
}

double LPAstar::GetCost(Cell from, Cell to) const {
    return linecost * euclid_heuristic(from, to);
}

std::vector<double> LPAstar::CalculateKey(Node vertex, const Map &map) {
    std::vector<double> res;
    res.push_back(std::min(vertex.g, vertex.rhs + heuristic(vertex.point, map.goal, map)));
    res.push_back(std::min(vertex.g, vertex.rhs));
    return res;
}

LPAstar::LPAstar() { OPEN.open_size = 0; }
LPAstar::~LPAstar()
{
    if (!NODES.empty()) NODES.erase(NODES.begin(), NODES.end());
}


Result LPAstar::FindThePath(const Map &map, Algorithm alg)
{

    linecost = alg.linecost;
    Initialize(map);

    if(!ComputeShortestPath(map))
        std::cout << "OOOPS\n";
    else
        std::cout << "Done\n";
    std::cout << "now\n";
    return current_result;
    //map.DamageTheMap(path);

}


void LPAstar::Initialize(const Map &map)
{
    Node start_node = Node(map.start);
    Node goal_node = Node(map.goal);
    start_node.clear_state();
    goal_node.clear_state();
    start_node.h = heuristic(map.start, map.goal, map);
    start_node.rhs = 0;
    NODES[vertex(map.start, map.height)] = start_node;
    start = &(NODES.find(vertex(map.start, map.height))->second);

    NODES[vertex(map.goal, map.height)] = goal_node;
    goal = &(NODES.find(vertex(map.goal, map.height))->second);

    OPEN.resize(map.width);
    OPEN.put(start);
    current_result = Result(false, 0, 0, 0);
}

void LPAstar::UpdateVertex(Node *vertex, const Map &map)
{
    if (!(vertex->point == map.start)) {
        double min_val = std::numeric_limits<double>::max();
        for (auto elem : GetSuccessors(vertex, map)) {
            if (elem->g + GetCost(elem->point, vertex->point) < min_val) {
                min_val = elem->g + GetCost(elem->point, vertex->point);
                vertex->rhs = min_val;
            }
        }
    }
    OPEN.remove_if(vertex);
    if (vertex->IsConsistent())
    {
        std::vector<double> key = CalculateKey(*vertex, map);
        vertex->key = key;
        OPEN.put(vertex);
    }
}

bool LPAstar::ComputeShortestPath(const Map &map)
{
    std:: cout << OPEN.top_key()[0] << std::endl;
    while (OPEN.top_key() < CalculateKey(*goal, map) || goal->rhs != goal->g) {
        Node *current = OPEN.get();
        std::cout << current->point << std::endl;
        if (current->point == map.goal) {
            goal = current;
            current_result = Result(true, OPEN.open_size + NODES.size(), 0, current->g);
            MakePrimaryPath(*current);
            return true;
        }
        if (current->g > current->rhs) {
            current->g = current->rhs;
            for (auto elem : GetSuccessors(current, map)) {
                UpdateVertex(elem, map);
            }
        } else {
            current->g = std::numeric_limits<double>::max();
            for (auto elem : GetSuccessors(current, map)) {
                UpdateVertex(elem, map);
            }
        }
    }
    return false;
}

std::vector<Node*> LPAstar::GetSuccessors(Node *current, const Map &map) {
    std::vector<Node*> result;
    for(auto elem : FindNeighbors(current, map)) {
        if(!NODES.count(vertex(elem.point, map.height))) {
            elem.g = current->g + GetCost(current->point, elem.point);
            NODES[vertex(elem.point, map.height)] = elem;
            result.push_back(&(NODES.find(vertex(elem.point, map.height))->second));
        } else {
            result.push_back(&(NODES.find(vertex(elem.point, map.height))->second));
        }
    }
    return result;
}

std::vector<Node> LPAstar::FindNeighbors(Node *n, const Map &map) const {
    std::vector<Node> result;
    int x = n->point.x;
    int y = n->point.y;
    if(map.algorithm_info.allowdiagonal) {
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
    return result;
}

void LPAstar::MakePrimaryPath(Node curNode)
{
    Node current = curNode;
    while (current.parent) {
        path.push_front(current);
        current = *current.parent;
    }
    path.push_front(current);
}
