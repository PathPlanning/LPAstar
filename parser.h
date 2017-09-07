#ifndef PARSER
#define PARSER

#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"
#include "gl.const.h"
#include <unordered_map>
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>
#include <random>

//general functions to convert information to abd from xml

//errors processing functions
inline void exit_error(const char *tag) {
    std::cout << "ERROR: Reading map failed. Not foung tag '" << tag <<  "'" << std::endl;
    exit(0);
}
inline void warning(const char *tag) {
    std::cout << "WARNING: Not foung tag '" << tag <<  "'. Using default value." << std::endl;
}

template <typename T>
int convert_to_int(T a) {
    std::istringstream myStream(a);
    int uintVar = 0;
    myStream >> uintVar;
    return uintVar;
}

template <typename T>
double convert_to_double(T a) {
    double var;
    var = std::stod(a);
    return var;
}

inline std::string get_info(TiXmlElement *b) {
    if(b) {
        const char *b_value = b->GetText();
        return b_value;
    }
    return "-1";
}

template<typename T>
TiXmlText * convert_to_xml(T a) {
    std::stringstream ss;
    ss << a;
    TiXmlText *text = new TiXmlText(ss.str().c_str());
    return text;
}

template<typename T>
std::string convert_to_string(T a) {
    std::stringstream so;
    so << a;
    std::string c = so.str();
    return c;
}

template<typename T>
const char * convert_to_char(T a) {
    std::ostringstream sc;
    sc << a;
    const char * c = sc.str().c_str();
    return c;
}

inline TiXmlText * convert_string(int * a, int len) {
    std::string st = "";
    for (int i = 0; i < len-1; i++)
        st += convert_to_string(a[i]) + ' ';
    st += convert_to_string(a[len - 1]);
    const char * c = st.c_str();
    TiXmlText *text = new TiXmlText(c);
    return text;
}

#endif // PARSER

