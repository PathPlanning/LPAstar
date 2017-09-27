#ifndef OPENLIST_H
#define OPENLIST_H

#include "node.h"
#include "gl_const.h"
#include <list>
#include <vector>

//representation of OPEN list as vector of lists, which provides both fast access to the element with minimum key value and fast search by the point value
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

    inline size_t size_of_open() const {
        return open_size;
    }

    inline Node* get() { //return node wit minimum key value
        Node* best;
        Key bkey = Key(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
        int coord = 0;
        for (size_t i = 0; i < elements.size(); i++) {
            if (!elements[i].empty()) {
                if (elements[i].front()->key < bkey) {
                    bkey = elements[i].front()->key;
                    coord = i;
                }
            }
        }
        best = elements[coord].front();
        return best;
    }
    void pop() { //pop the node with minimum key value
        Key bkey = Key(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
        int coord = 0;
        for (size_t i = 0; i < elements.size(); i++) {
            if (!elements[i].empty()) {
                if (elements[i].front()->key < bkey) {
                    bkey = elements[i].front()->key;
                    coord = i;
                }
            }
        }
        elements[coord].pop_front();
        --open_size;
    }

    inline Key top_key() { //return the minimum key value
        Key best_key(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
        for (size_t i = 0; i < elements.size(); i++) {

            if (!elements[i].empty()) {
                if (elements[i].front()->key < best_key) {
                   best_key = elements[i].front()->key;
                }
            }
        }
        return best_key;
    }
    inline  bool top_key_less_than(Key cur_key) { //compare the minimum key value and current key value
        bool exists = false;
        Key best_key(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
        for (size_t i = 0; i < elements.size(); i++) {
            if (!elements[i].empty()) {
                exists = true;
                if (elements[i].front()->key < best_key) {
                    best_key = elements[i].front()->key;
                }
            }
        }
        if(!exists) return false;
        return best_key < cur_key;
    }

    inline void put (Node* item) { //add node to OPEN list or renew it's key value, is it is already there
        if (elements[item->point.y].empty()) {
            elements[item->point.y].emplace_back(item);
            ++open_size;
            return;
        }
        elements[item->point.y].remove_if([item](Node* curr) { return curr->point == item->point; });
        std::list<Node*>::iterator it = elements[item->point.y].begin();
        std::list<Node*>::iterator position = elements[item->point.y].end();
        while (it != elements[item->point.y].end()) {
            if ((*it)->key < item->key) {
                position = it;
            }
            ++it;
        }
        if (++position != elements[item->point.y].end()) elements[item->point.y].emplace(position, item);
        else elements[item->point.y].emplace_back(item);
        ++open_size;
    }

    inline void remove_if(Node* item) {
        elements[item->point.y].remove_if([item](Node* curr) { return curr->point == item->point; });
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
                    std::cout << (*it)->point << (*it)->key.k1 << " ";
                }
                std::cout << std::endl;
            } else std::cout << "None\n";
        }
    }
    void set_size(int s) { open_size = 0; }

private:
    std::vector<std::list<Node*> > elements;
    size_t open_size;

};

#endif // OPENLIST_H
