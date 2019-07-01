#pragma once
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

// Classic helper function
class Util {

public:

static std::string convertToTime ( long int input_seconds );
static std::string getProgressBar(std::string percent);
static void getStream(std::string path, std::ifstream& stream);
static char* getCString(std::string cpp_str);
static bool isNumeric(std::string str);
static std::vector<std::string> split(const std::string& str, char delim);
static std::vector<std::string> splitFields(const std::string& str, int delim (int));
static void printFields(const std::vector<std::string>& fields);
static std::string trim(const std::string& str, const char * exclude);
}; // end class Util

/************************************************************/
std::string Util::convertToTime (long int input_seconds){
long minutes = input_seconds / 60;
long hours = minutes / 60;
long seconds = int(input_seconds%60);
minutes = int(minutes%60);
std::string result = std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds);
return result;
}
// constructing string for given percentage
// 50 bars is uniformly streched 0 - 100 %
// meaning: every 2% is one bar(|)
std::string Util::getProgressBar(std::string percent){

    std::string result = "0%% ";
    int _size= 50;
    int  boundaries;
    try {
        boundaries = (stof(percent)/100)*_size;
    } catch (...){
    boundaries = 0;
    }

    for(int i=0;i<_size;i++){
        if(i<=boundaries){
        result +="|";
        }
        else{
        result +=" ";
        }
    }

    result +=" " + percent.substr(0,5) + " /100%%";
    return result;
}

// wrapper for creating streams
void Util::getStream(std::string path, std::ifstream& stream){
    stream.open (path, std::ifstream::in);
    if (!stream && !stream.is_open()){
        stream.close();
        throw std::runtime_error("Non-existent PID");
    }
    //return stream;
}
// Convert std::string to C-string
char* Util::getCString(std::string str){
    char * cstr = new char [str.length()+1];
    std::strcpy (cstr, str.c_str());
    return cstr;
}
// Check whether string is numerical (i.e., only digits) 
bool Util::isNumeric(std::string str){
    for (char& c: str){
        if (!std::isdigit(c)){
            return false;
        }
    }
    return true;
}
//Print elements of a string vector
void Util::printFields(const std::vector<std::string>& fields){
    for (std::string f: fields){
        std::cout << f << "||";
    }
    std::cout << std::endl;
}
//Trim leading and trailing characters
std::string Util::trim(const std::string& str, const char * excl)
{
    size_t first = str.find_first_not_of(excl);
    if (first == std::string::npos)
        return str;
    size_t last = str.find_last_not_of(excl);
    return str.substr(first, (last - first + 1));
}
// Split string into tokens at specified delimiter
std::vector<std::string> Util::split(const std::string& str, char delim){
    std::vector<std::string> tokens;
    std::string tok;
    std::istringstream tokstream(str);
    while (std::getline(tokstream, tok, delim))
    {
        tokens.push_back(tok);
    }
    return tokens;
}
// Split into fields separated by delimiters 
std::vector<std::string> Util::splitFields(const std::string& str, int delim (int) = ::isspace){
    std::vector<std::string> fields;
    auto end = str.end();
    auto index = str.begin();
    while (index != end)
    {
        index = find_if_not(index, end, delim);
        if (index == end) break;
        //inside the token, look for next delimiter
        auto next_boundary = find_if(index, end, delim);
        fields.push_back(std::string( index, next_boundary));
        index = next_boundary;
    }
    return fields;
}
