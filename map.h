#ifndef MAP_H
#define	MAP_H
#include <iostream>
#include "node.h"
#include <list>
#include "parser.h"
#include "structures.h"
#include <sstream>
#include <string>
#include <algorithm>

class Map
{
    public:
        Map();
        Map(const Map& orig);
        ~Map();

        void DamageTheMap(std::list<Node> path);
        bool GetMap(const char *name);
        bool CellIsTraversable (Cell curr) const;
        bool CellOnGrid (Cell curr) const;
        bool CellIsObstacle(Cell cur) const;
        bool CellIsNeighbor(Cell next, Cell curr) const;

        bool Cut(Cell next, Cell current) const;
        bool Squeeze(Cell next, Cell current) const;

        int * operator [] (int i);
        const int * operator [] (int i) const;

        int     height, width;
        Cell    start;
        Cell    goal;
        Algorithm algorithm_info;

    private:
        double  CellSize;
        int **  Grid;

        void BuildGrid();

        std::string filename;

};

#endif
