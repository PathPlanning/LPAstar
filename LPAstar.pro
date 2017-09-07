TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    map.cpp \
    tinyxml/tinystr.cpp \
    tinyxml/tinyxml.cpp \
    tinyxml/tinyxmlerror.cpp \
    tinyxml/tinyxmlparser.cpp \
    lpastar.cpp

HEADERS += \
    map.h \
    gl.const.h \
    node.h \
    openlist.h \
    structures.h \
    tinyxml/tinystr.h \
    tinyxml/tinyxml.h \
    parser.h \
    lpastar.h \
    heuristics.h
