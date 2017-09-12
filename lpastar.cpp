#include "lpastar.h"


inline int vertex(Cell item, int size) {
    return item.x * size + item.y;
}

double LPAstar::GetCost(Cell from, Cell to, Map &map) const {
    if (map.CellIsNeighbor(to, from))
        return linecost * euclid_heuristic(from, to);
    else
        return std::numeric_limits<double>::infinity();
}

std::vector<double> LPAstar::CalculateKey(Node vertex, Map &map) {
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
    std::cout << "now\n";
    map.DamageTheMap(path);
    std::vector<Node> damaged = {Node(Cell(0, 4)), Node(Cell(1, 4)), Node(Cell(2,4))};
    //Node changed1 = NODES.find(vertex(Cell(1,5), map.height))->second;
    //UpdateVertex(changed, map);
    std::set<Node> update;
    for (auto elem : damaged) {
        for (auto el : GetSuccessors(elem, map)) {
           update.insert(el);
        }
    }
    for (auto elem: update) UpdateVertex(elem, map);
    if(ComputeShortestPath(map)){
        std::cout << "ALL OK\n";
    } else
        std::cout << "NOT OK\n";
}


void LPAstar::Initialize(Map &map)
{
    Node start_node = Node(map.start);
    Node goal_node = Node(map.goal);
    start_node.rhs = 0;
    start_node.g = std::numeric_limits<double>::infinity();
    start_node.key = CalculateKey(start_node, map);

    goal_node.g = std::numeric_limits<double>::infinity();
    goal_node.rhs = std::numeric_limits<double>::infinity();

    NODES[vertex(map.goal, map.height)] = goal_node;
    goal = &(NODES.find(vertex(map.goal, map.height))->second);

    OPEN.resize(map.width);
    OPEN.put(start_node);
    current_result = Result(false, 0, 0, 0);
}

void LPAstar::CloseOpen(double height) {
    for (auto elem : OPEN.get_elements()) {
        NODES[vertex(elem.point, height)] = elem;
    }
}

void LPAstar::UpdateVertex(Node vert, Map &map)
{
    if (!(vert.point == map.start)) {
        double min_val = std::numeric_limits<double>::infinity();
        for (auto elem : GetSuccessors(vert, map)) {
            if (elem.g + GetCost(elem.point, vert.point, map) < min_val) {
                min_val = elem.g + GetCost(elem.point, vert.point, map);
                vert.rhs = min_val;
                //std::cout << elem.point << std::endl;

                vert.parent = &(NODES.find(vertex(elem.point, map.height))->second);
                std::cout << "changed: " << vert.point << " " << elem.point << std::endl;
            }
        }
    }
    OPEN.remove_if(vert);
    if (!vert.IsConsistent()) {
        vert.key = CalculateKey(vert, map);
        OPEN.put(vert);
    }
}

bool LPAstar::ComputeShortestPath(Map &map)
{
    while (OPEN.top_key_less_than(CalculateKey(*goal, map)) || goal->rhs != goal->g) {
        //OPEN.print_elements();
        Node current = OPEN.get();
        //NODES[vertex(current.point, map.height)] = current;
        if (current.g > current.rhs) {
            current.g = current.rhs;
            current.opened = true;
            NODES[vertex(current.point, map.height)] = current;
            for (auto elem : GetSuccessors(current, map)) {
                UpdateVertex(elem, map);
            }
        } else {
            current.g = std::numeric_limits<double>::infinity();
            current.opened = true;
            NODES[vertex(current.point, map.height)] = current;
            for (auto elem : GetSuccessors(current, map)) {
                UpdateVertex(elem, map);
            }
            UpdateVertex(current, map);
        }
        if(current.parent) {
            std::cout << current.point << "g " << current.g << " rhs" << current.rhs <<
                  current.parent->point << std::endl;
        }
        std::cout << OPEN.top_key()[0] << ',' << OPEN.top_key()[1] << std::endl;
        if (current.point == map.goal) {
            NODES[vertex(current.point, map.height)] = current;
            for (auto elem : NODES) elem.second.opened = false;
            goal = &(NODES.find(vertex(current.point, map.height))->second);
            std::cout << "goal" << goal->point << goal->rhs << " "<< goal->g <<std::endl;
        }

    }
    if (goal->g != std::numeric_limits<double>::infinity()) {
        current_result = Result(true, OPEN.open_size + NODES.size(), 0, goal->g);
        std::cout << "nowww\n";
        std::cout << goal->g << std::endl;
        MakePrimaryPath(*goal);
        CloseOpen(map.height);
        return true;
    }
    return false;
}

std::vector<Node> LPAstar::GetPredecessors(Node current, Map &map) {
    std::vector<Node> result;
    //std::cout << current.point << std::endl;
    for(auto elem : FindNeighbors(current, map)) {
        if(NODES.count(vertex(elem.point, map.height))) { //if vertex wasn't previously examined
            Node node = NODES.find(vertex(elem.point, map.height))->second;
            //if (node.opened) {
                result.push_back(node);
            //}
        }
    }
    return result;
}


std::vector<Node> LPAstar::GetSuccessors(Node current, Map &map) {
    std::vector<Node> result;
    for(auto elem : FindNeighbors(current, map)) {
        if(!NODES.count(vertex(elem.point, map.height))) { //if vertex wasn't previously examined
            elem.g =  std::numeric_limits<double>::infinity();
            elem.rhs = std::numeric_limits<double>::infinity();
            result.push_back(elem);
        } else {
            Node node = NODES.find(vertex(elem.point, map.height))->second;
            result.push_back(node);
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
