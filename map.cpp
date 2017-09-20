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

int * Map::operator [] (int i) {
    return Grid[i];
}
const int * Map::operator [] (int i) const {
    return Grid[i];
}

bool Map::CellIsTraversable(Cell curr) const
{
    return (Grid[curr.y][curr.x] == CN_GC_NOOBS);
}

bool Map::CellIsObstacle(Cell curr) const
{
    return (Grid[curr.y][curr.x] != CN_GC_NOOBS);
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

Changes Map::DamageTheMap(std::list<Node> path)
{
    Changes result;
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(3, path.size() - 3); // guaranteed unbiased

    auto random_number = uni(rng);
    //random_number = path.size() - 2;
    int i = 0;
    Node crash = path.front();
    auto it = path.begin();
    while (it != path.end()) {
        if (i++ == random_number) {
            crash = *it;
            break;
        }
        ++it;
    }
    int x = crash.point.x;
    int y = crash.point.y;
    for (int k = y - 1; k <= y + 1; ++k) {
        for (int l = x - 1; l <= x + 1; ++l) {
            if (CellOnGrid(Cell(l, k)) && CellIsTraversable(Cell(l, k)) && Cell(l,k) != goal && Cell(l,k) != start) {
                result.occupied.push_back(Cell(l, k));
                Grid[k][l] = CN_GC_OBS;
            }
        }
    }

    x = crash.point.x + 1;
    y = crash.point.y - 1;
    for (int k = y - 1; k <= y + 1; ++k) {
        for (int l = x - 1; l <= x + 1; ++l) {
            if (CellOnGrid(Cell(l, k)) && CellIsTraversable(Cell(l, k)) && Cell(l,k) != goal && Cell(l,k) != start) {
                result.occupied.push_back(Cell(l, k));
                Grid[k][l] = CN_GC_OBS;
            }
        }
    }
    /*x = crash.point.x - 1;
    y = crash.point.y + 1;
    for (int k = y - 1; k <= y + 1; ++k) {
        for (int l = x - 1; l <= x + 1; ++l) {
            if (CellOnGrid(Cell(l, k)) && CellIsTraversable(Cell(l, k)) && Cell(l,k) != goal && Cell(l,k) != start) {
                result.occupied.push_back(Cell(l, k));
                Grid[k][l] = CN_GC_OBS;
            }
        }
    }
    x = crash.point.x + 1;
    y = crash.point.y + 1;
    for (int k = y - 1; k <= y + 1; ++k) {
        for (int l = x - 1; l <= x + 1; ++l) {
            if (CellOnGrid(Cell(l, k)) && CellIsTraversable(Cell(l, k)) && Cell(l,k) != goal && Cell(l,k) != start) {
                result.occupied.push_back(Cell(l, k));
                Grid[k][l] = CN_GC_OBS;
            }
        }
    }
    x = crash.point.x - 1;
    y = crash.point.y - 1;
    for (int k = y - 1; k <= y + 1; ++k) {
        for (int l = x - 1; l <= x + 1; ++l) {
            if (CellOnGrid(Cell(l, k)) && CellIsTraversable(Cell(l, k)) && Cell(l,k) != goal && Cell(l,k) != start) {
                result.occupied.push_back(Cell(l, k));
                Grid[k][l] = CN_GC_OBS;
            }
        }
    }*/
    /*for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            std::cout << Grid[i][j] << " ";
        }
        std::cout << std::endl;
    }*/
    return result;
}

void Map::PrintPath(std::list<Node> path) {
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            bool p = false;
            for (auto elem : path) {
                if (elem.point == Cell(j,i)) {
                    std::cout << "* ";
                    p = true;
                    break;
                }
            }
            if(!p) std::cout << Grid[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

bool Map::GetMap(const char *FileName)
{
    int rowiter = 0, grid_i = 0, grid_j = 0;

    tinyxml2::XMLElement *root = 0, *map = 0, *element = 0, *mapnode;

    std::string value;
    std::stringstream stream;

    bool hasGridMem = false, hasGrid = false, hasHeight = false, hasWidth = false, hasSTX = false, hasSTY = false, hasFINX = false, hasFINY = false, hasCellSize = false;

    tinyxml2::XMLDocument doc;

    // Load XML File
    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS) {
        std::cout << "Error opening XML file!" << std::endl;
        return false;
    }
    // Get ROOT element
    root = doc.FirstChildElement(CNS_TAG_ROOT);
    if (!root) {
        std::cout << "Error! No '" << CNS_TAG_ROOT << "' tag found in XML file!" << std::endl;
        return false;
    }

    // Get MAP element
    map = root->FirstChildElement(CNS_TAG_MAP);
    if (!map) {
        std::cout << "Error! No '" << CNS_TAG_MAP << "' tag found in XML file!" << std::endl;
        return false;
    }

    for (mapnode = map->FirstChildElement(); mapnode; mapnode = mapnode->NextSiblingElement()) {
        element = mapnode->ToElement();
        value = mapnode->Value();
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);

        stream.str("");
        stream.clear();
        stream << element->GetText();

        if (!hasGridMem && hasHeight && hasWidth) {
            Grid = new int *[height];
            for (int i = 0; i < height; ++i)
                Grid[i] = new int[width];
            hasGridMem = true;
        }

        if (value == CNS_TAG_HEIGHT) {
            if (hasHeight) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_HEIGHT << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_HEIGHT << "' =" << height << "will be used."
                          << std::endl;
            }
            else {
                if (!((stream >> height) && (height > 0))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_HEIGHT
                              << "' tag encountered (or could not convert to integer)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_HEIGHT << "' tag should be an integer >=0" << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_HEIGHT
                              << "' tag will be encountered later..." << std::endl;
                }
                else
                    hasHeight = true;
            }
        }
        else if (value == CNS_TAG_WIDTH) {
            if (hasWidth) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_WIDTH << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_WIDTH << "' =" << width << "will be used." << std::endl;
            }
            else {
                if (!((stream >> width) && (width > 0))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_WIDTH
                              << "' tag encountered (or could not convert to integer)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_WIDTH << "' tag should be an integer AND >0" << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_WIDTH
                              << "' tag will be encountered later..." << std::endl;

                }
                else
                    hasWidth = true;
            }
        }
        else if (value == CNS_TAG_CELLSIZE) {
            if (hasCellSize) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_CELLSIZE << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_CELLSIZE << "' =" << CellSize << "will be used."
                          << std::endl;
            }
            else {
                if (!((stream >> CellSize) && (CellSize > 0))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_CELLSIZE
                              << "' tag encountered (or could not convert to double)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_CELLSIZE
                              << "' tag should be double AND >0. By default it is defined to '1'" << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_CELLSIZE
                              << "' tag will be encountered later..." << std::endl;
                }
                else
                    hasCellSize = true;
            }
        }
        else if (value == CNS_TAG_STX) {
            if (!hasWidth) {
                std::cout << "Error! '" << CNS_TAG_STX << "' tag encountered before '" << CNS_TAG_WIDTH << "' tag."
                          << std::endl;
                return false;
            }

            if (hasSTX) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_STX << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_STX << "' =" << start.x << "will be used." << std::endl;
            }
            else {
                if (!(stream >> start.x && start.x >= 0 && start.x < width)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_STX
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_STX << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_WIDTH << "' value, which is " << width << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_STX
                              << "' tag will be encountered later..." << std::endl;
                }
                else
                    hasSTX = true;
            }
        }
        else if (value == CNS_TAG_STY) {
            if (!hasHeight) {
                std::cout << "Error! '" << CNS_TAG_STY << "' tag encountered before '" << CNS_TAG_HEIGHT << "' tag."
                          << std::endl;
                return false;
            }

            if (hasSTY) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_STY << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_STY << "' =" << start.y << "will be used." << std::endl;
            }
            else {
                if (!(stream >> start.y && start.y >= 0 && start.y < height)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_STY
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_STY << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_HEIGHT << "' value, which is " << height << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_STY
                              << "' tag will be encountered later..." << std::endl;
                }
                else
                    hasSTY = true;
            }
        }
        else if (value == CNS_TAG_FINX) {
            if (!hasWidth) {
                std::cout << "Error! '" << CNS_TAG_FINX << "' tag encountered before '" << CNS_TAG_WIDTH << "' tag."
                          << std::endl;
                return false;
            }

            if (hasFINX) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_FINX << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_FINX << "' =" << goal.x << "will be used." << std::endl;
            }
            else {
                if (!(stream >> goal.x && goal.x >= 0 && goal.x < width)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_FINX
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_FINX << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_WIDTH << "' value, which is " << width << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_FINX
                              << "' tag will be encountered later..." << std::endl;
                }
                else
                    hasFINX = true;
            }
        }
        else if (value == CNS_TAG_FINY) {
            if (!hasHeight) {
                std::cout << "Error! '" << CNS_TAG_FINY << "' tag encountered before '" << CNS_TAG_HEIGHT << "' tag."
                          << std::endl;
                return false;
            }

            if (hasFINY) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_FINY << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_FINY << "' =" << goal.y << "will be used." << std::endl;
            }
            else {
                if (!(stream >> goal.y && goal.y >= 0 && goal.y < height)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_FINY
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_FINY << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_HEIGHT << "' value, which is " << height << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_FINY
                              << "' tag will be encountered later..." << std::endl;
                }
                else
                    hasFINY = true;
            }
        }
        else if (value == CNS_TAG_GRID) {
            hasGrid = true;
            if (!(hasHeight && hasWidth)) {
                std::cout << "Error! No '" << CNS_TAG_WIDTH << "' tag or '" << CNS_TAG_HEIGHT << "' tag before '"
                          << CNS_TAG_GRID << "'tag encountered!" << std::endl;
                return false;
            }
            element = mapnode->FirstChildElement();
            while (grid_i < height) {
                if (!element) {
                    std::cout << "Error! Not enough '" << CNS_TAG_ROW << "' tags inside '" << CNS_TAG_GRID << "' tag."
                              << std::endl;
                    std::cout << "Number of '" << CNS_TAG_ROW
                              << "' tags should be equal (or greater) than the value of '" << CNS_TAG_HEIGHT
                              << "' tag which is " << height << std::endl;
                    return false;
                }
                std::string str = element->GetText();
                std::vector<std::string> elems;
                std::stringstream ss(str);
                std::string item;
                while (std::getline(ss, item, ' '))
                    elems.push_back(item);
                rowiter = grid_j = 0;
                int val;
                if (elems.size() > 0)
                    for (grid_j = 0; grid_j < width; ++grid_j) {
                        if (grid_j == elems.size())
                            break;
                        stream.str("");
                        stream.clear();
                        stream << elems[grid_j];
                        stream >> val;
                        Grid[grid_i][grid_j] = val;
                    }

                if (grid_j != width) {
                    std::cout << "Invalid value on " << CNS_TAG_GRID << " in the " << grid_i + 1 << " " << CNS_TAG_ROW
                              << std::endl;
                    return false;
                }
                ++grid_i;

                element = element->NextSiblingElement();
            }
        }
    }
    //some additional checks
    if (!hasGrid) {
        std::cout << "Error! There is no tag 'grid' in xml-file!\n";
        return false;
    }
    if (!(hasFINX && hasFINY && hasSTX && hasSTY))
        return false;

    if (Grid[start.y][start.x] != CN_GC_NOOBS) {
        std::cout << "Error! Start cell is not traversable (cell's value is" << Grid[start.y][start.x] << ")!"
                  << std::endl;
        return false;
    }

    if (Grid[goal.y][goal.x] != CN_GC_NOOBS) {
        std::cout << "Error! Goal cell is not traversable (cell's value is" << Grid[goal.y][goal.x] << ")!"
                  << std::endl;
        return false;
    }

    return true;
}
