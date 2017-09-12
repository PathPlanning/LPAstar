#include "map.h"

Map::Map()
{
    height = -1;
    width = -1;
    start = Cell(-1,-1);
    goal = Cell(-1,-1);
    Grid = NULL;
    CellSize = 1;
}

Map::~Map()
{
    if (Grid) {
        for (int i = 0; i < height; ++i)
            delete[] Grid[i];
        delete[] Grid;
    }
}

bool Map::CellIsTraversable(Cell curr) const
{
    return (Grid[curr.y][curr.x] == CN_CELL_IS_NOT_BLOCKED);
}

bool Map::CellIsObstacle(Cell curr) const
{
    return (Grid[curr.y][curr.x] != CN_CELL_IS_NOT_BLOCKED);
}

bool Map::CellOnGrid(Cell curr) const
{
    return (curr.y < height && curr.y >= 0 && curr.x < width && curr.x >= 0);
}

bool Map::Squeeze(Cell next, Cell current) const {
    if (next.x == current.x || next.y == current.y) return 1;
    if (algorithm_info.allowsqueeze) return 1;
    if (next.x + 1 == current.x && next.y - 1 == current.y)
        return !(Grid[current.y][current.x - 1] && Grid[current.y + 1][current.x]);
    if (next.x + 1 == current.x && next.y + 1 == current.y)
        return !(Grid[current.y][current.x - 1] && Grid[current.y - 1][current.x]);
    if (next.x - 1 == current.x && next.y + 1 == current.y)
        return !(Grid[current.y][current.x + 1] && Grid[current.y - 1][current.x]);
    if (next.x - 1 == current.x && next.y - 1 == current.y)
        return !(Grid[current.y][current.x + 1] && Grid[current.y + 1][current.x]);
    return 0;
}

bool Map::Cut(Cell next, Cell current) const {
    if (next.x == current.x || next.y == current.y) return 1;
    if (algorithm_info.cutcorners) return 1;
    if (next.x + 1 == current.x && next.y - 1 == current.y)
        return !(Grid[current.y][current.x - 1] || Grid[current.y + 1][current.x]);
    if (next.x + 1 == current.x && next.y + 1 == current.y)
        return !(Grid[current.y][current.x - 1] || Grid[current.y - 1][current.x]);
    if (next.x - 1 == current.x && next.y + 1 == current.y)
        return !(Grid[current.y][current.x + 1] || Grid[current.y - 1][current.x]);
    if (next.x - 1 == current.x && next.y - 1 == current.y)
        return !(Grid[current.y][current.x + 1] || Grid[current.y + 1][current.x]);
    return 0;
}

bool Map::CellIsNeighbor(Cell next, Cell current) const
{
    return CellOnGrid(next) && CellIsTraversable(next)
            && Squeeze(next, current) && Cut(next, current);
}

void Map::BuildGrid() {
    Grid = new int * [height];
    for(int count = 0; count < height; ++count){
        Grid[count] = new int [width];
    }
}

void Map::DamageTheMap(std::list<Node> path)
{
    Grid[4][0] = 1;
    Grid[4][1] = 1;
    Grid[4][2] = 1;

    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            std::cout << Grid[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

bool Map::GetMap(const char *name)
{
    TiXmlDocument doc(name);
    try {
        if(doc.LoadFile()) {
            std::cout << "Load sucsses" << std::endl;
        } else {
            throw "ERROR: Load failed. Try another file";
        }
    }
    catch (const char * exc) {
        std::cout << exc << std::endl;
        exit(0);
    }
    filename = name;
    TiXmlElement *root = doc.FirstChildElement(CNS_ROOT);
    if(root) {
        TiXmlElement *map = root->FirstChildElement(CNS_MAP);
        if(map) {
            TiXmlElement *twidth = map->FirstChildElement(CNS_WIDTH);
            if(twidth) {
                const char *width_value = twidth->GetText();
                width =  convert_to_int(width_value);
            } else exit_error(CNS_WIDTH);
            TiXmlElement *theight = map->FirstChildElement(CNS_HEIGHT);
            if(theight) {
                const char *height_value = theight->GetText();
                height =  convert_to_int(height_value);
            } else exit_error(CNS_WIDTH);
            TiXmlElement *cellsize = map->FirstChildElement(CNS_CELLSIZE);
            if(cellsize) {
                const char *cellsize_value = cellsize->GetText();
                CellSize =  convert_to_int(cellsize_value);
            } else {
                warning(CNS_CELLSIZE);
                CellSize = 1;
            }
            TiXmlElement *startx = map->FirstChildElement(CNS_STARTX);
            if(startx) {
                const char *startx_value = startx->GetText();
                start.x = convert_to_int(startx_value);
            } else {
                warning(CNS_STARTX);
                start.x = 0;
            }
            TiXmlElement *starty = map->FirstChildElement(CNS_STARTY);
            if(starty) {
                const char *starty_value = starty->GetText();
                start.y = convert_to_int(starty_value);
            } else {
                warning(CNS_STARTY);
                start.y = 0;
            }
            TiXmlElement *finishx = map->FirstChildElement(CNS_FINISHX);
            if(finishx) {
                const char *finishx_value = finishx->GetText();
                goal.x =  convert_to_int(finishx_value);
            } else {
                warning(CNS_FINISHX);
                goal.x = 0;
            }
            TiXmlElement *finishy = map->FirstChildElement(CNS_FINISHY);
            if(finishy) {
                const char *finishy_value = finishy->GetText();
                goal.y =  convert_to_int(finishy_value);
            } else {
                warning(CNS_FINISHY);
                goal.y = 0;
            }
            BuildGrid();
            TiXmlElement *g = map->FirstChildElement(CNS_GRID);
            if(g) {
                int i = 0;
                TiXmlElement *row = g->FirstChildElement(CNS_ROW);
                if (row) {
                    for (row; row; row = row->NextSiblingElement(CNS_ROW)) {
                        const char *number = row->Attribute(CNS_NUMBER);
                        if (number) i = convert_to_int(number);
                        const char *rowvalue = row->GetText();
                        int count = 0;
                        for (int j = 0; j < width; ++j) {
                            int elem = rowvalue[count] - '0';
                            Grid[i][j] = elem;
                            count += 2;
                        }
                        ++i;
                        if (i > height) {
                            std::cout << "ERROR: the size of map does not match parametr" << std::endl;
                            exit(0);
                        }
                    }
                    if (i < height) {
                        std::cout << "ERROR: the size of map does not match parametr" << std::endl;
                        exit(0);
                    }
                }
            } else exit_error(CNS_GRID);
            std::cout << "Reading map sucsses" << std::endl;
        } else exit_error(CNS_MAP);
        TiXmlElement *algorithm = root->FirstChildElement(CNS_ALGORITHM);
        if (algorithm) {
            TiXmlElement *searchtype = algorithm->FirstChildElement(CNS_SEARCHTYPE);
            if (searchtype) {
                algorithm_info.searchtype = get_info(searchtype);
            } else exit_error(CNS_SEARCHTYPE);
            TiXmlElement * metrictype = algorithm->FirstChildElement(CNS_METRICTYPE);
            if (metrictype) {
                algorithm_info.metrictype = get_info(metrictype);
            } else warning(CNS_METRICTYPE);
            TiXmlElement * hweight = algorithm->FirstChildElement(CNS_HWEIGHT);
            if (hweight) {
                const char *hweight_value = hweight->GetText();
                algorithm_info.hweight = convert_to_double(hweight_value);
            } else {
                warning(CNS_HWEIGHT);
                algorithm_info.hweight = 1;
            }
            TiXmlElement * breakingties = algorithm->FirstChildElement(CNS_BREAK);
            if (breakingties) {
                algorithm_info.breakingties = get_info(breakingties);
            } else warning(CNS_BREAK);
            TiXmlElement * linecost = algorithm->FirstChildElement(CNS_LINE);
            if (linecost) {
                const char *linecost_value = linecost->GetText();
                algorithm_info.linecost = convert_to_double(linecost_value);
            } else {
                warning(CNS_LINE);
                algorithm_info.linecost = 1;
            }
            TiXmlElement * diagonalcost = algorithm->FirstChildElement(CNS_DIAGONAL);
            if (diagonalcost) {
                const char *diagonal_value = diagonalcost->GetText();
                algorithm_info.diagonalcost = convert_to_double(diagonal_value);
            } else {
                warning(CNS_DIAGONAL);
                algorithm_info.diagonalcost = sqrt(2);
            }

            TiXmlElement * allowdiagonal = algorithm->FirstChildElement(CNS_ALLOW);
            if (allowdiagonal) {
                const char *allowdiag_value = allowdiagonal->GetText();
                if (strstr(allowdiag_value, "true") != nullptr
                        || strstr(allowdiag_value, "1") != nullptr) {
                    algorithm_info.allowdiagonal = true;
                }
                else algorithm_info.allowdiagonal = false;
            } else {
                warning(CNS_ALLOW);
                algorithm_info.allowdiagonal = false;
            }
            TiXmlElement * allowsqueeze = algorithm->FirstChildElement(CNS_ALLOWS);
            if (allowsqueeze) {
                const char *allowsq_value = allowsqueeze->GetText();
                if(strstr(allowsq_value,"true") != nullptr
                        || strstr(allowsq_value,"1") != nullptr) algorithm_info.allowsqueeze = true;
                else algorithm_info.allowsqueeze = false;
            } else {
                warning(CNS_ALLOWS);
                algorithm_info.allowsqueeze = false;
            }
            TiXmlElement *cutcorners = algorithm->FirstChildElement(CNS_CUTCORNERS);
            if (cutcorners) {
                const char *cut_value = cutcorners->GetText();
                if(strstr(cut_value,"true") != nullptr
                        || strstr(cut_value, "1") != nullptr) algorithm_info.cutcorners = true;
                else algorithm_info.cutcorners = false;
            } else {
                warning(CNS_CUTCORNERS);
                algorithm_info.cutcorners = false;
            }
        }

        TiXmlElement *options = root->FirstChildElement(CNS_OPTIONS);
        if(options) {

            TiXmlElement *loglevel = options->FirstChildElement(CNS_LOGLEVEL);
            if (loglevel) {
                algorithm_info.loglevel = convert_to_double(get_info(loglevel));
            }

            TiXmlElement *logpath = options->FirstChildElement(CNS_LOGPATH);
            if (logpath) {
                if(logpath->GetText()) algorithm_info.logpath = convert_to_string(get_info(logpath));
            }

            TiXmlElement *logfilename = options->FirstChildElement(CNS_LOGFILENAME);
            if (logfilename && logfilename->GetText()) {
                algorithm_info.logfilename = convert_to_char(get_info(logfilename));
            } else {
                std::cout << "WARNING: Not foung tag '" << CNS_LOGFILENAME <<  "'. Using default value." << std::endl;
                std::stringstream ss;
                ss << name;
                std::string file_name = ss.str();
                if(file_name.rfind(".")) {
                    file_name.replace(file_name.rfind("."), 1, "_log.");
                } else {
                    file_name += "_log.xml";
                }
                algorithm_info.logfilename = file_name;
            }
        }
    } else exit_error(CNS_ROOT);
    std::cout << "goooo\n";
}
