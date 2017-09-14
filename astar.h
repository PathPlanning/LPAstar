#ifndef ASTAR_H
#define ASTAR_H

#include "ilogger.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include <list>
#include <unordered_map>
#include <vector>
#include <math.h>
#include <limits>
#include <chrono>


class Astar
{
    public:
        Astar();
        Astar(double HW, bool BT);
        virtual ~Astar(void);

        SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options);

    protected:
        ANode findMin();
        void addOpen(ANode newNode);
        double computeHFromCellToCell(int start_i, int start_j, int fin_i, int fin_j, const EnvironmentOptions &options);
        std::list<ANode> findSuccessors(ANode curNode, const Map &map, const EnvironmentOptions &options);
        void makePrimaryPath(ANode curNode);//Makes path using back pointers
        void makeSecondaryPath();//Makes another type of path(sections or points)
        ANode resetParent(ANode current, ANode parent, const Map &map, const EnvironmentOptions &options) {return current;}//Function for Theta*
        bool stopCriterion();

        SearchResult                    sresult;
        std::list<ANode>                 lppath, hppath;
        std::unordered_map<int,ANode>    close;
        std::vector<std::list<ANode>>    open;
        int                             openSize;
        double                          hweight;//weight of h-value
        bool                            breakingties;//flag that sets the priority of nodes in addOpen function when their F-values is equal

};

#endif
