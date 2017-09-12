#ifndef OPENLIST_H
#define OPENLIST_H

#include "node.h"
#include "gl.const.h"
#include <list>
#include <vector>

class OpenList {
public:

    inline bool empty() const {
        return !open_size;
    }

    inline size_t size_l() const {
        return open_size;
    }

    inline Node get() {
        Node best;
        std::vector<double> key;
        key.resize(2, std::numeric_limits<double>::infinity());
        int coord = 0;
        for (size_t i = 0; i < elements.size(); i++) {
            if (!elements[i].empty()) {
                if (elements[i].front().key[0] < key[0]) {
                    if (elements[i].front().key[0] == key[0]) {
                        if(elements[i].front().key[1] <= key[1]) {
                            key = elements[i].front().key;
                            coord = i;
                        }
                    } else {
                        key = elements[i].front().key;
                        coord = i;
                    }
                }
            }
        }
        best = elements[coord].front();
        elements[coord].pop_front();
        --open_size;
        return best;
    }
    inline std::vector<double> top_key() {
        std::vector<double> best_key;
        best_key.resize(2, std::numeric_limits<double>::infinity());

        for (size_t i = 0; i < elements.size(); i++) {

            if (!elements[i].empty()) {
                if (elements[i].front().key[0] <= best_key[0]) {
                    if(elements[i].front().key[0] == best_key[0]) {
                        if (elements[i].front().key[1] <= best_key[1])
                             best_key = elements[i].front().key;
                    } else {
                        best_key = elements[i].front().key;
                    }
                }
            }
        }
        return best_key;
    }
    inline  bool top_key_less_than(std::vector<double> cur_key) {
        std::vector<double> best_key;
        best_key.resize(2, std::numeric_limits<double>::infinity());

        for (size_t i = 0; i < elements.size(); i++) {
            if (!elements[i].empty()) {
                if (elements[i].front().key[0] <= best_key[0]) {
                    if(elements[i].front().key[0] == best_key[0]) {
                        if (elements[i].front().key[1] <= best_key[1])
                            best_key = elements[i].front().key;
                    } else {
                        best_key = elements[i].front().key;
                    }
                }
            }
        }
        return best_key[0] < cur_key[0] || ( best_key[0] == cur_key[0] && best_key[1] <= cur_key[1] );
    }

    inline void put (Node item) {
        std::list<Node>::iterator it = elements[item.point.x].begin();
        std::list<Node>::iterator position = elements[item.point.x].end();
        if (elements[item.point.x].empty()) {
            elements[item.point.x].emplace_back(item);
            ++open_size;
            return;
        }
        while (it != elements[item.point.x].end()) {
            if (it->key[0] < item.key[0]) {
                position = it;
            } else if (it->key[0] == item.key[0] && it->key[1] <= item.key[1]) {
                position = it;
            }
            ++it;
        }
        if (++position != elements[item.point.x].end()) elements[item.point.x].emplace(position, item);
        else elements[item.point.x].emplace_back(item);
        ++open_size;
    }

    inline void remove_if(Node item) {
        if (!elements[item.point.x].empty()) {
            //std::cout << elements[item.point.x].front().point << std::endl;
            auto it = elements[item.point.x].begin();
            while (it != elements[item.point.x].end()) {
                //std::cout << it->point << std::endl;
                if (it->point == item.point) {
                    //std::cout << "removed " << it->point << std::endl;
                    elements[item.point.x].erase(it);

                }
                ++it;
            }
        }
    }

    inline void resize(int value) {
        elements.resize(value);
    }

    std::vector<Node> get_elements() const {
        std::vector<Node> result;
        for (auto elem : elements) {
            for(auto it = elem.begin(); it != elem.end(); ++it) {
                result.push_back(*it);
            }
        }
        return result;
    }

    void print_elements() const {
        for (auto elem : elements) {
            if (!elem.empty()) {
                for(auto it = elem.begin(); it != elem.end(); ++it) {
                    std::cout << it->point << " ";
                }
                std::cout << std::endl;
            }
        }
    }

    std::vector<std::list<Node>> elements;
    size_t open_size;

};

#endif // OPENLIST_H
