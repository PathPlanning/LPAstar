#ifndef OPENLIST_H
#define OPENLIST_H

#include "node.h"
#include "gl_const.h"
#include <list>
#include <vector>

//representation of OPEN list as vector of lists, which provides both fast access to the element with minimum key value and fast search by the cell value
class OpenList {
public:

    ~OpenList() {
        for (auto elem : elements) {
            if(!elem.empty()) elem.erase(elem.begin(), elem.end());
        }
    }

    inline bool empty() const {
        for (auto elem : elements) {
            if(!elem.empty()) return false;
        }
        return true;
    }

    inline Node* get() { //return node wit minimum key value
        Node* best = elements[current_top_coord].front();
        if (best->g > best->rhs) elements[current_top_coord].pop_front();
        return best;
    }

    inline  bool top_key_less_than(Key cur_key) { //compare the minimum key value and current key value
        bool exists = false;
        Key best_key(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
        for (size_t i = 0; i < elements.size(); i++) {
            if (!elements[i].empty()) {
                exists = true;
                if (elements[i].front()->key < best_key) {
                    best_key = elements[i].front()->key;
                    current_top_coord = i;
                }
            }
        }
        if(!exists) return false;
        return best_key < cur_key;
    }

    inline void put (Node* item) { //add node to OPEN list or renew it's key value, is it is already there
        if (elements[item->cell.y].empty()) {
            elements[item->cell.y].emplace_back(item);
            return;
        }
        //elements[item->cell.y].remove_if([item](Node* curr) { return curr->cell == item->cell; });
        std::list<Node*>::iterator it = elements[item->cell.y].begin();
        std::list<Node*>::iterator position = elements[item->cell.y].end();
        bool found_position = false;
        while (it != elements[item->cell.y].end()) {
            if (!found_position && !((*it)->key < item->key)) {
                position = it;
                found_position = true;
            }
            if ((*it)->cell == item->cell) {
                elements[item->cell.y].erase(it);
                --it;
            }
            ++it;
        }
        if (found_position) elements[item->cell.y].emplace(position, item);
        else elements[item->cell.y].emplace_back(item);
    }

    inline void remove_if(Node* item) {
        elements[item->cell.y].remove_if([item](Node* curr) { return curr->cell == item->cell; });
    }

    inline void resize(int value) {
        elements.resize(value);
    }

    std::vector<Node> get_elements() const {
        std::vector<Node> result;
        for (auto elem : elements) {
            for(auto it = elem.begin(); it != elem.end(); ++it) {
                result.push_back(*(*it));
            }
        }
        return result;
    }

    void print_elements() const {
        for (auto elem : elements) {
            if (!elem.empty()) {
                for(auto it = elem.begin(); it != elem.end(); ++it) {
                    std::cout << (*it)->cell << (*it)->key.k1 << " ";
                }
                std::cout << std::endl;
            } else std::cout << "None\n";
        }
    }

private:
    std::vector<std::list<Node*> > elements;
    int current_top_coord;

};

#endif // OPENLIST_H
