#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <ncurses.h>
#include <time.h>
#include <sstream>
#include <iomanip>
#include "util.h"
#include "SysInfo.h"
// #include "ProcessContainer.h"
// #include "ProcessParser.h"
// #include "Process.h"

using namespace std;

int main( int argc, char *argv[] )
{
    // ProcessParser procParser;
    // Get list of all PIDs
    std::vector<string> pidList = ProcessParser::getPidList();
    int counter = 0;
    int pidcount = pidList.size();
    for (int i=0; i < pidcount; i++){
        counter++;
        std::cout << pidList[i] << " ";
        if (counter > 10){
            std::cout << std::endl;
            counter = 0;
        }
    }
    std::cout << std::endl;
    // 
    SysInfo sys();

    //Select a PID and get some data
    string some_pid = "14715"; //pidList[pidcount/2];
    string non_pid = to_string(stoi(pidList[pidcount - 1]) + 1000);
    //Check whether PID exists
    //Existing PID
    std::cout << "Does PID " << some_pid << " exist? " << ProcessParser::isPidExisting(some_pid) << std::endl;
    std::cout << std::endl;
    //Non-existent PID
    std::cout << "Does PID " << non_pid << " exist? (expect false) " << ProcessParser::isPidExisting(non_pid) << std::endl; 
    std::cout << std::endl; 

    //Command name 
    std::cout << "PID: " << some_pid << "\n\n";
    std::cout<< "Command Name: " << ProcessParser::getCmd(some_pid) << std::endl;
    std::cout << std::endl;
    //VmData
    std::cout << "Total Memory (GB): " << ProcessParser::getVmSize(some_pid) << std::endl;
    std::cout << std::endl;
    //User ID
    std::cout << "UID: " << ProcessParser::getProcUid(some_pid) << std::endl;
    std::cout << std::endl;
    // User name
    std::cout << "User: " << ProcessParser::getProcUser(some_pid) << std::endl;
    std::cout << std::endl;
    // Number of cpu cores on host 
    std::cout << "Number of CPU Cores: " << ProcessParser::getNumberOfCores() << std::endl;
    std::cout << std::endl;
    // OS Name
    std::cout<< "OS Name: " << ProcessParser::getOSName() << std::endl;
    std::cout << std::endl;
    // Kernel Version 
    std::cout<< "Kernel Version: " << ProcessParser::getSysKernelVersion() << std::endl;
    std::cout << std::endl;
    // System uptime
    std::cout<< "System Uptime (sec): " << ProcessParser::getSysUpTime() << std::endl;
    std::cout << std::endl;
    // Process uptime
    std::cout<< "Process Uptime (sec): " << ProcessParser::getProcUpTime(some_pid) << std::endl;
    std::cout << std::endl;
    // Percentage CPU 
    std::cout<< "CPU Percentage: " << ProcessParser::getCpuPercent(some_pid) << std::endl;
    std::cout << std::endl;
    // CPU information
    std::vector<string> cpu2 = ProcessParser::getSysCpuPercent("2");
    std::cout<< "CPU Info: ";
    Util::printFields(cpu2);
    std::cout << std::endl;
    std::vector<string> cpu4 = ProcessParser::getSysCpuPercent("4");
    std::cout<< "CPU Info: ";
    Util::printFields(cpu4);
    std::cout << std::endl;
    std::vector<string> cpu = ProcessParser::getSysCpuPercent();
    std::cout<< "CPU Info: ";
    Util::printFields(cpu);
    std::cout << std::endl;
    // RAM %
    std::cout<< "RAM %: " << ProcessParser::getSysRamPercent() << " %" << std::endl;
    std::cout << std::endl;
    // CPU stats
    std::vector<string> v1 = ProcessParser::getSysCpuPercent();
    std::this_thread::sleep_for(std::chrono::milliseconds(15000));
    std::vector<string> v2 = ProcessParser::getSysCpuPercent();
    std::cout<< "CPU Stats: " << ProcessParser::printCpuStats(v1, v2) << " %" << std::endl;
    std::cout << std::endl;

    Process proc(some_pid);
    std::cout << proc.getProcess() << std::endl;
}
