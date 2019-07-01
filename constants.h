#pragma once
#include <string>
using namespace std;

/* CPU information in /proc/stat:
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
 */
enum CPUStates{
	S_USER = 1,
	S_NICE,
	S_SYSTEM,
	S_IDLE,
	S_IOWAIT,
	S_IRQ,
	S_SOFTIRQ,
	S_STEAL,
	S_GUEST,
	S_GUEST_NICE
};

class Path{

public:
    static string basePath() {
        return "/proc/";
    }
    static string cmdPath(){
        return "/cmdline";
    }
    static string statusPath(){
        return "/status";
    }
    static string statPath(){
        return "stat";
    }
    static string upTimePath(){
        return "uptime";
    }
    static string memInfoPath(){
        return "meminfo";
    }
    static string versionPath(){
        return "version";
    }
};
