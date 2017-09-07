#include <iostream>
#include "lpastar.h"

int main(int argc, char *argv[])
{
    try {
        if(!argv[1]) {
            throw "No input file. Please try again";
        }
    }
    catch (const char *exc) {
        std::cout << exc << std::endl;
        return 0;
    }
    char *file_name = argv[1];
    Map map;
    map.GetMap(file_name);
    LPAstar path;
    std::cout << "what?\n";
    Result res = path.FindThePath(map, map.algorithm_info);
    std::cout << res.length << std::endl;
    return 0;
}

