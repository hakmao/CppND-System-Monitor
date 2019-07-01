#pragma once
#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"

using namespace std;

class ProcessParser {
private:
    std::ifstream stream;
    static double getSysActiveCpuTime(vector<string> values);
    static double getSysIdleCpuTime(vector<string>values);
public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static std::string getVmSize(string pid);
    static std::string getCpuPercent(string pid);
    static long int getSysUpTime();
    static std::string getProcUpTime(string pid);
    static string getProcUid(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string core_num = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getNumberOfCores();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static std::string printCpuStats(std::vector<std::string> val1, std::vector<std::string>val2);
    static bool isPidExisting(string pid);
};
// TODO: Define all of the above functions below:
string ProcessParser::getCmd(string pid)
{
    string path = Path::basePath() + pid + Path::cmdPath();
    ifstream stream; 
    Util::getStream(path, stream);
    // Get the only line in the file
    string cmdline;
    std::getline(stream, cmdline);
    return cmdline;
}
vector<string> ProcessParser::getPidList()
{
    DIR* dir;
    vector<string> pid_list;
    if(!(dir = opendir("/proc")))
        throw std::runtime_error(std::strerror(errno));

    while (dirent* dirp = readdir(dir)) {
        // Check whether file is a directory
        if(dirp->d_type != DT_DIR)
            continue;
        // Check whether the file name is numeric (i.e., only digits) 
        // if (all_of(dirp->d_name, dirp->d_name + std::strlen(dirp->d_name), [](char c){ return std::isdigit(c); })) {
        if (Util::isNumeric(dirp->d_name)){
            pid_list.push_back(dirp->d_name);
        }
    }
    if(closedir(dir))
        throw std::runtime_error(std::strerror(errno));
    return pid_list;
}
string ProcessParser::getVmSize(string pid)
{
    // NOTE: "VmSize" corresponds to total current process memory usage
    // (cf. "VmData" is the size of the data segment)
    string match = "VmSize:";
    // declare value to be returned
    float vmsize = 0;
    // Create stream from path to status file
    string path = Path::basePath() + pid + Path::statusPath();
    ifstream stream;
    Util::getStream(path, stream);
    string line;
    while(std::getline(stream, line)){
        //Split into fields
        std::vector<string> fields = Util::splitFields(line);
        // Search for line containing "VmSize"
        if (match == fields[0]) {
            vmsize = stof(fields[1]); // Size in kB
            vmsize = vmsize/float(1024*1024); //Convert from kB to GB
            break;
        }
    }
    return to_string(vmsize);
}
// Get process user ID  from /proc/pid/status
string ProcessParser::getProcUid(string pid){
    string target = "Uid:";
    // Create stream from process 'status' file 
    string path = Path::basePath() + pid + Path::statusPath();
    ifstream stream;
    Util::getStream(path, stream);
    string line;
    while(std::getline(stream, line)){
        //Split line into tokens using tabs as delimiter
        std::vector<string> tokens = Util::splitFields(line);
        // Util::printFields(tokens);
        // Search for line containing "Uid:" as the first token
        if (target == tokens[0]){
            //Found a match, so return second token in the list
            return tokens[1];
        }
    }
    return ""; //No match
}
// Get user name from /etc/passwd using UID
string ProcessParser::getProcUser(string pid){
    string uid = getProcUid(pid);
    string path = "/etc/passwd";
    ifstream stream;
    Util::getStream(path, stream);
    string line;
    while(std::getline(stream, line)){
        //split the line into tokens using ':" as delimiter 
        std::vector<string> tokens = Util::split(line, ':');
        // Util::printFields(tokens);
        //compare the third token in the list with the supplied PID
        if (uid == tokens[2])
        {
            //Found a match, so return the first token in the list 
            return tokens[0];
        }
    }
    return ""; //no matches found
}

int ProcessParser::getNumberOfCores()
{
    string target = "cpu cores";
    string path = Path::basePath() + "cpuinfo";
    ifstream stream;
    Util::getStream(path, stream);
    string line;
    while (std::getline(stream, line)) {
        std::vector<string> tokens = Util::split(line, ':');
        // Util::printFields(tokens);
        std::string property = tokens[0];
        if (property.compare(0, target.size(), target) == 0)
        {
            //No. of cores is second token in the list
            return stoi(tokens[1]);
        }
    }
    return 0;
}

string ProcessParser::getOSName()
{
    //First try '/etc/os-release', which should be present on all systemd distros
    string path = "/etc/os-release";
    ifstream stream; 
    Util::getStream(path, stream);
    string match = "PRETTY_NAME";
    string line;
    string os_name = "";
    while (std::getline(stream, line)){
        std::vector<string> fields = Util::split(line, '=');
        if (match == fields[0]){
            os_name = fields[1];
            //Remove double quotes 
            if (os_name.length() > 2){
                if (os_name.front() == '"')
                    os_name.erase(0,1);
                if (os_name.back() == '"')
                    os_name.erase(os_name.length() - 1);
            }
            return os_name;
        }
    }
    //On a non-systemd distribution, '/etc/os-release' may not be present,
    //so let's use the bare OS type in '/proc/sys/kernel/ostype' as backup
    path = Path::basePath() + "sys/kernel/ostype";
    std::getline(stream, os_name); //There's only one line in the file
    return os_name;
}

string ProcessParser::getSysKernelVersion()
{
    // This is the source forthe kernel version in /proc/version 
    string path = Path::basePath() + "sys/kernel/osrelease";
    ifstream stream; 
    Util::getStream(path, stream);
    // Get the only line in the file
    string version;
    std::getline(stream, version);
    return version;
}

long int ProcessParser::getSysUpTime()
{
    string path = Path::basePath() + Path::upTimePath();
    ifstream stream; 
    Util::getStream(path, stream);
    string line;
    std::getline(stream, line);
    // There is only one line in the file, of which we want the first entry 
    std::vector<std::string> tokens = Util::splitFields(line);
    string seconds = tokens[0]; //Return uptime in seconds
    return stol(seconds); 
}
string ProcessParser::getProcUpTime(string pid)
{
    string path = Path::basePath() + pid + "/stat";
    ifstream stream;
    Util::getStream(path, stream);
    string line;
    std::getline(stream, line); // just one line
    std::vector<std::string> fields = Util::splitFields(line);
    // std::cout << "Stat fields: " << std::endl;
    // Util::printFields(fields);
    // Number of clock ticks elapsed is given by the 14th field in the line,
    // which corresponds to index 13 in the vector of tokens.
    //  Note that the value is given as an unsigned long,
    //  indicated by its proper scanf format specifier "%lu",
    // but we will convert this to 'long double' for the number of seconds
    // --see proc(5)
    long double ticks = stold(fields[13]);
    // Get the frequency in ticks per second 
    long double ticks_per_second = sysconf(_SC_CLK_TCK);
    // Calculate number of seconds
    long double seconds = ticks / ticks_per_second;
    // return to_string(float(stof(values[13])/ticks_per_second));
    return  to_string( seconds);
}

std::string ProcessParser::getCpuPercent(string pid){
    string path = Path::basePath() + pid + "/stat";
    ifstream stream;
    Util::getStream(path, stream);
    string line;
    std::getline(stream, line); // just one line
    std::vector<std::string> fields = Util::splitFields(line);
    // #14: utime  - user mode, unsigned long int 
    long int utime = stol(fields[13]); // %lu
    // #15: stime  - kernel mode, unsigned long int  
    long int stime = stol(fields[14]); // %lu
    // #16: cutime - children (user mode), long int  
    long int cutime = stol(fields[15]); // %ld
    // #17: cstime - children (kernel mode), long int  
    long int cstime = stol(fields[16]); // %ld
    long int proc_start_time = stol(fields[21]); //ticks
    long int uptime = ProcessParser::getSysUpTime(); // seconds!!
    // Convert seconds back into ticks, so we have the same units
    long int sys_uptime = uptime * sysconf(_SC_CLK_TCK); //ticks
    //Calculate percentage
    //Note type conversion to long double for percentage calculation
    long double total_proc_uptime = utime + stime + cutime + cstime; //ticks
    // std::cout << "Total process uptime: " << total_proc_uptime << std::endl;
    long double system_time_elapsed = sys_uptime - proc_start_time; //ticks
    //Calculate the ratio (doesn't matter if it's ticks or seconds)
    long double cpu_percent= 100.00 * ( total_proc_uptime / system_time_elapsed );
    return to_string(cpu_percent);
}
/*********************************************************** 
* getSysCpuPercent uses the information in file /proc/stat

CPU information in /proc/stat:
=============================
user    (1) Time spent in user mode.
nice    (2) Time spent in user mode with low priority (nice).
system  (3) Time spent in system mode.
idle    (4) Time spent in the idle task.  (== USER_HZ * #2 in /proc/uptime) 
iowait  (5) Time waiting for I/O to complete. (not reliable)
irq     (6) Time servicing interrupts.
softirq (7) Time servicing softirqs.
steal   (8) "Stolen" time (virtualized environments)
guest   (9) guest operating systems 
guest_nice (10)  Time spent running a "niced" guest
**************************************************************/
std::vector<string> ProcessParser::getSysCpuPercent(string core_num)
{
    // Read the data for specified CPU core and return it as a vector
    // If no individual core is passed, then we read the "cpu" line
    string core = "cpu" + core_num;
    string path = Path::basePath() + Path::statPath();
    ifstream stream;
    Util::getStream(path, stream);
    string line;
    while (std::getline(stream, line)) {
        //Tokenise
        std::vector<string> fields = Util::splitFields(line);
        // Util::printFields(tokens);
        std::string name = fields[0];       
        if (name == core){
            return fields;
        }
    }
    return (std::vector<string>());
}
// Auxiliary functions to extract system cpu information from vector
double ProcessParser::getSysActiveCpuTime(vector<string> values)
{
    return (stod(values[S_USER]) +
            stod(values[S_NICE]) +
            stod(values[S_SYSTEM]) +
            stod(values[S_IRQ]) +
            stod(values[S_SOFTIRQ]) +
            stod(values[S_STEAL]) +
            stod(values[S_GUEST]) +
            stod(values[S_GUEST_NICE]));
}
double ProcessParser::getSysIdleCpuTime(vector<string>values)
{
    return (stod(values[S_IDLE]) + stod(values[S_IOWAIT]));
}
/*
CPU stats are calculated based on measurements taken at two points in time.
The function "printCpuStats" therefore takes as parameters two vectors of values.
*/
string ProcessParser::printCpuStats(vector<string> values1, vector<string> values2)
{
    double active = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
    double idle = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
    double total = active + idle;
    double percent_of_total = 100.0*(active / total);
    return to_string(percent_of_total);
}
/*
This function returns the percentage of total system RAM in use. 

Htop employs the following colour codes:
- Green: Used memory pages
- Blue: Buffer pages
- Yellow: Cache pages

Htop employs the following calculations --cf. LinuxProcessList.c, Platform.c

a. Total used memory = MemTotal - MemFree
b. Non cache/buffer memory (Green) = Total used memory - (Buffers + Cached memory)
c. Buffers (Blue) = Buffers
d. Cached memory (Yellow) = Cached + SReclaimable - Shmem
e. Swap = SwapTotal - SwapFree

These values are of type 'long int'. However, we will need to convert them to floating point for the percentage calculation, and return a 'float' in accordance with the specified interface declaration of the getSysRamPercent() function.

We can ignore Swap memory in calculating RAM usage, so we need to calculate Cached memory (Yellow), Buffers (Blue), Non-cache/buffer memory (Green), as well as total used memory.
*/
float ProcessParser::getSysRamPercent()
{
    int attribute_counter = 6;
    string mem_total_attr= "MemTotal:";
    string mem_free_attr= "MemFree:";
    string buffers_attr = "Buffers:";
    string cached_attr = "Cached:";
    string sreclaimable_attr = "SReclaimable:";
    string shmem_attr = "Shmem:";

    double mem_total = 0;
    double mem_free = 0;
    double buffers = 0;
    double cached = 0;
    double sreclaimable = 0;
    double shmem = 0;

    // Read data from file
    string path = Path::basePath() + Path::memInfoPath();
    ifstream stream;
    Util::getStream(path, stream);
    string line;
    while (std::getline(stream, line)) {
        if (attribute_counter <= 0)
            break;
        //Split line into tokens
        std::vector<string> fields = Util::splitFields(line);
        // std::cout << "Fields: " << std::endl;
        // Util::printFields(fields);
        // Match against property names
        if (fields[0] == mem_total_attr){
            mem_total = stod(fields[1]);
            // std::cout << mem_total_attr << ": " << mem_total << std::endl;
            --attribute_counter;
        }
        else if (fields[0] == mem_free_attr){
            mem_free = stod(fields[1]);
            // std::cout << mem_free_attr << ": " << mem_free  << std::endl;
            --attribute_counter;
        }
        else if (fields[0] == buffers_attr){
            buffers = stod(fields[1]);
            // std::cout << buffers_attr << ": " << buffers  << std::endl;
            --attribute_counter;
        }
        else if (fields[0] == cached_attr){
            cached = stod(fields[1]);
            // std::cout << cached_attr << ": " << cached  << std::endl;
            --attribute_counter;
        }
        else if (fields[0] == sreclaimable_attr){
            sreclaimable = stod(fields[1]);
            // std::cout << sreclaimable_attr << ": " << sreclaimable << std::endl;
            --attribute_counter;
        }
        else if (fields[0] == shmem_attr){
            shmem = stod(fields[1]);
            // std::cout << shmem_attr << ": " << shmem << std::endl;
            --attribute_counter;
        }
    }
    if (attribute_counter > 0){
        std::cout << "ERROR: Number of attributes not read properly == " << attribute_counter << std::endl;
    }
    //Calculate memory usage (long double)
    double total_used_memory = mem_total - mem_free;
    double cached_memory = cached + sreclaimable - shmem; //Yellow
    double memory_used = total_used_memory - (buffers + cached_memory); //Green 
    //Calculate percentage (truncation should be OK as we are calculating ratio)
    // std::cout << "RAM Total Memory Used: " << total_used_memory << std::endl;
    // std::cout << "RAM Cached Memory: " << cached_memory << std::endl;
    // std::cout << "RAM Memory Used: " << memory_used << std::endl;
    double ratio_memory_used = memory_used / mem_total;
    float percentage_memory_used = 100.00 * ratio_memory_used;
    return percentage_memory_used;
}

int ProcessParser::getTotalNumberOfProcesses()
{
    int total_processes = 0;
    string target = "processes";
    string path = Path::basePath() + Path::statPath();
    ifstream stream;
    Util::getStream(path, stream);
    string line;
    while (std::getline(stream, line)) {
        std::vector<string> fields = Util::splitFields(line);
        if (fields[0] == target) {
            total_processes += stoi(fields[1]);
            break;
        }
    }
    return total_processes;
}
int ProcessParser::getNumberOfRunningProcesses()
{
    int procs_running = 0;
    string target = "procs_running";
    string path = Path::basePath() + Path::statPath();
    ifstream stream;
    Util::getStream(path, stream);
    string line;
    while (std::getline(stream, line)) {
        std::vector<string> fields = Util::splitFields(line);
        if (fields[0] == target) {
            procs_running += stoi(fields[1]);
            break;
        }
    }
    return procs_running;
}
int ProcessParser::getTotalThreads()
{
    int total_threads = 0;
    string target = "Threads:";
    vector<string> pid_list = ProcessParser::getPidList();
    for (int i=0 ; i< pid_list.size(); i++) {
        //Get each process
        string pid = pid_list[i];
        //Read the number of threads
        string path = Path::basePath() + pid + Path::statusPath();
        ifstream stream;
        Util::getStream(path, stream);
        string line;
        while (std::getline(stream, line)) {
            std::vector<string> fields = Util::splitFields(line);
            if (target == fields[0]) {
                total_threads += stoi(fields[1]);
                break;
            }
        }
    }
    return total_threads;
}

bool ProcessParser::isPidExisting(string pid){
    vector<string> pid_list = getPidList();
    return std::any_of(pid_list.begin(), pid_list.end(), [&pid](string p){ return pid == p;});
}
