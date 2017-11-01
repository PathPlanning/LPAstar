#include "mission.h"
#include "astar.h"
#include "xmllogger.h"
#include "gl_const.h"

Mission::Mission()
{
    logger = nullptr;
    fileName = nullptr;
    LPAmatchesA = false;
}

Mission::Mission(const char *FileName)
{
    fileName = FileName;
    logger = nullptr;
    LPAmatchesA = false;
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
}

void Mission::createSearch()
{
    lpasearch = LPAstar(config.SearchParams[CN_SP_HW]);
    search = Astar(config.SearchParams[CN_SP_HW], config.SearchParams[CN_SP_BT]);
}

void Mission::startSearch()
{
    lparesult = lpasearch.FindThePath(map, options);
    if (!lparesult.goal_became_unreachable) {
        sr = search.startSearch(logger, map, options);
        if (!lparesult.pathfound && !sr.pathfound) {
            LPAmatchesA = true;
            std::cout << "For both LPAstar and Astar algorithms path does not exist\n";
        }
        if (lparesult.pathlength == sr.pathlength) {
            LPAmatchesA = true;
            std::cout << "LPAstar is correct: LPAstar path length matches Astar path length\n";
        }
    } else {
        LPAmatchesA = true;
    }

}

void Mission::printSearchResultsToConsole()
{
    std::cout << "LPApath ";
    if (!lparesult.pathfound)
        std::cout << "NOT ";
    std::cout << "found!" << std::endl;
    std::cout << "Apath ";
    if (!sr.pathfound)
        std::cout << "NOT ";
    std::cout << "found!" << std::endl;

    std::cout << "numberofsteps: LPAstar=" << lparesult.numberofsteps << ", Astar=" << sr.numberofsteps << std::endl;
    std::cout << "nodescreated: LPAstar=" << lparesult.nodescreated << ", Astar=" << sr.nodescreated << std::endl;

    if (sr.pathfound && lparesult.pathfound) {
        std::cout << "pathlength: LPAstar="  << lparesult.pathlength << ", Astar=" << sr.pathlength << std::endl;
        std::cout << "pathlength_scaled: LPAstar=" <<  lparesult.pathlength * map.get_cellsize() << ", Astar=" <<  sr.pathlength * map.get_cellsize() << std::endl;
    }
    std::cout << "time: LPAstar=" << lparesult.time << ", Astar=" <<  sr.time << std::endl;
}

void Mission::saveSearchResultsToLog()
{
    logger->writeToLogSummary(lparesult.numberofsteps, lparesult.nodescreated, lparesult.pathlength, lparesult.time, map.get_cellsize(), sr.pathlength, LPAmatchesA);
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
