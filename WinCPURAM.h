#ifndef WINCPURAM_H
#define WINCPURAM_H

#include "windows.h"
#include "psapi.h"
class WinCPURAM
{
public:
    WinCPURAM();

    unsigned int getTotalVirtualMemoryUsed();
    unsigned int getTotalVirtualMemory();

    unsigned int getTotalPhysicalMemoryUsed();
    unsigned int getTotalPhysicalMemory();

    unsigned int getVirtualMemory();//used by process
    unsigned int getPhysicalMemory();//used by process

    double getCPU();

private:
    ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    int numProcessors;
    HANDLE self;
    MEMORYSTATUSEX memInfo;
    PROCESS_MEMORY_COUNTERS_EX pmc;
};

#endif // WINCPURAM_H
