CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11 -O2 -Wall -Wextra

win32 {
QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
}


SOURCES += main.cpp \
    map.cpp \
    lpastar.cpp \
    astar.cpp \
    config.cpp \
    environmentoptions.cpp \
    mission.cpp \
    xmllogger.cpp \
    tinyxml2.cpp

HEADERS += \
    map.h \
    node.h \
    openlist.h \
    structures.h \
    lpastar.h \
    heuristics.h \
    astar.h \
    config.h \
    environmentoptions.h \
    ilogger.h \
    mission.h \
    searchresult.h \
    xmllogger.h \
    tinyxml2.h \
    gl_const.h
