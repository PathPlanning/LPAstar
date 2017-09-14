#ifndef ILOGGER_H
#define	ILOGGER_H
#include "map.h"
#include <node.h>
#include <unordered_map>
#include <list>

class ILogger
{
    public:
        ILogger(std::string loglevel) {this->loglevel = loglevel;}
        virtual bool getLog(const char* FileName, const std::string* LogParams) = 0;
        virtual void saveLog() = 0;
        virtual void writeToLogMap(const Map& map, const std::list<Node>& path) = 0;
        virtual void writeToLogOpenClose(const std::vector<std::list<ANode>>& open, const std::unordered_map<int,ANode>& close, bool last) = 0;
        virtual void writeToLogPath(const std::list<Node>& path) = 0;
        virtual void writeToLogHPpath(const std::list<Node>& path) = 0;
        virtual void writeToLogNotFound() = 0;
        virtual void writeToLogSummary(unsigned int numberofsteps, unsigned int nodescreated, float length, double time, double cellSize, float alength, bool acorrect) = 0;
        virtual ~ILogger() {};
    protected:
        std::string loglevel;
};

#endif

