#include "mission.h"
#include "xmllogger.h"
#include "gl_const.h"

Mission::Mission()
{
    logger = nullptr;
    fileName = nullptr;
    correct = false;
}

Mission::Mission(const char *FileName)
{
    fileName = FileName;
    logger = nullptr;
    correct = false;
}

Mission::~Mission()
{
    if (logger)
        delete logger;
}

bool Mission::getMap()
{
    return map.GetMap(fileName);
}

bool Mission::getConfig()
{
    return config.getConfig(fileName);
}

bool Mission::createLog()
{
    if (logger != NULL) delete logger;
    logger = new XmlLogger(config.LogParams[CN_LP_LEVEL]);
    return logger->getLog(fileName, config.LogParams);
}

void Mission::createEnvironmentOptions()
{
    if (config.SearchParams[CN_SP_ST] == CN_SP_ST_BFS || config.SearchParams[CN_SP_ST] == CN_SP_ST_DIJK) {

        options = EnvironmentOptions(config.SearchParams[CN_SP_AS], config.SearchParams[CN_SP_AD],
                                     config.SearchParams[CN_SP_CC]);

    }

    else
        options = EnvironmentOptions(config.SearchParams[CN_SP_AS], config.SearchParams[CN_SP_AD],
                                     config.SearchParams[CN_SP_CC], config.SearchParams[CN_SP_MT]);
    map.algorithm_info = options;
}

void Mission::createSearch()
{
    lpasearch = LPAstar(config.SearchParams[CN_SP_HW]);
}
void Mission::startSearch()
{
    lparesult = lpasearch.FindThePath(map, options, config.changes);
}

void Mission::printSearchResultsToConsole()
{
    std::cout << "Path ";
    if (!lparesult.pathfound)
        std::cout << "NOT ";
    std::cout << "found!" << std::endl;
    std::cout << "numberofsteps=" << lparesult.numberofsteps << std::endl;
    std::cout << "nodescreated=" << lparesult.nodescreated << std::endl;
    if (lparesult.pathfound) {
        std::cout << "pathlength=" << lparesult.pathlength << std::endl;
        std::cout << "pathlength_scaled=" << lparesult.pathlength * map.CellSize << std::endl;
    }
    std::cout << "time=" << lparesult.time << std::endl;
}

void Mission::saveSearchResultsToLog()
{
    logger->writeToLogSummary(lparesult.numberofsteps, lparesult.nodescreated, lparesult.pathlength, lparesult.time, map.CellSize);
    if (lparesult.pathfound) {
        logger->writeToLogPath(*lparesult.lppath);
        logger->writeToLogHPpath(*lparesult.hppath);
        logger->writeToLogMap(map, *lparesult.lppath, true);
    } else {
        logger->writeToLogMap(map, *lparesult.lppath, false);
        logger->writeToLogNotFound();
    }
    logger->saveLog();
}
