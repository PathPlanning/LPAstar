cmake_minimum_required(VERSION 2.8)
project(LPAstar)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
set(SOURCE_FILES
	astar.cpp
        config.cpp
        environmentoptions.cpp
	lpastar.cpp
	main.cpp
        map.cpp
        mission.cpp
        tinyxml2.cpp
        xmllogger.cpp)

set(HEADER_FILES
	node.h
	astar.h
        config.h
        environmentoptions.h
	gl_const.h
        heuristics.h
	ilogger.h
	lpastar.h
        map.h
	mission.h
	openlist.h
        searchresult.h
	structures.h
        tinyxml2.h
        xmllogger.h)
add_executable(LPAstar ${SOURCE_FILES} ${HEADER_FILES})
