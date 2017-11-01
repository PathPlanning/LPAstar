#ifndef MAP_H
#define	MAP_H
#include <iostream>
#include "node.h"
#include "environmentoptions.h"
#include "gl_const.h"
#include <list>
#include "searchresult.h"
#include "tinyxml2.h"
#include <sstream>
#include <string>
#include <algorithm>
#include <random>


class Map
{
    public:
        Map();
        Map(const Map& orig);
        ~Map();

        Changes DamageTheMap(std::list<Node> path);
        Changes ClearTheMap(std::list<Cell> damaged);
        bool GetMap(const char *name);
        bool CellIsTraversable (Cell curr) const;
        bool CellOnGrid (Cell curr) const;
        bool CellIsObstacle(Cell cur) const;

        int * operator [] (int i);
        const int * operator [] (int i) const;

        void PrintPath(std::list<Node> path);

        const int get_height() const;
        const int get_width() const;
        Cell get_start() const;
        Cell get_goal() const;
        double get_cellsize() const;

    private:

        int **  Grid;

        void BuildGrid();
        Cell damaged;
        int     height, width;
        Cell    start;
        Cell    goal;
        double  CellSize;

        std::string filename;

};

#endif
