#ifndef MISSION_H
#define	MISSION_H

#include "map.h"
#include "config.h"
#include "ilogger.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include "astar.h"
#include "lpastar.h"
#include "xmllogger.h"

class Mission
{
    public:
        Mission();
        Mission (const char* fileName);
        ~Mission();

        bool getMap();
        bool getConfig();
        bool createLog();
        void createSearch();
        void createEnvironmentOptions();
        void startSearch();
        void printSearchResultsToConsole();
        void saveSearchResultsToLog();

    private:
        const char* getAlgorithmName();

        Map                     map;
        Config                  config;
        EnvironmentOptions      options;
        LPAstar                 lpasearch;
        Astar                   search;
        ILogger*                logger;
        const char*             fileName;
        SearchResult            sr;
        LPASearchResult         lparesult;
        bool                    LPAmatchesA;
};

#endif

