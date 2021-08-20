#include "WinCPURAM.h"


WinCPURAM::WinCPURAM()
{
    //Memory
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    //Cpu
    SYSTEM_INFO sysInfo;
    FILETIME ftime, fsys, fuser;
    GetSystemInfo(&sysInfo);
    numProcessors = sysInfo.dwNumberOfProcessors;
    GetSystemTimeAsFileTime(&ftime);
    memcpy(&lastCPU, &ftime, sizeof(FILETIME));
    self = GetCurrentProcess();
    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
    memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
}

unsigned int WinCPURAM::getTotalVirtualMemory()
{
    GlobalMemoryStatusEx(&memInfo);
    return static_cast<unsigned int>(memInfo.ullTotalPageFile);
}

unsigned int WinCPURAM::getTotalVirtualMemoryUsed()
{
    GlobalMemoryStatusEx(&memInfo);
    return static_cast<unsigned int>(memInfo.ullTotalPageFile - memInfo.ullAvailPageFile);
}

unsigned int WinCPURAM::getTotalPhysicalMemory()
{
    GlobalMemoryStatusEx(&memInfo);
    return static_cast<unsigned int>(memInfo.ullTotalPhys);
}

unsigned int WinCPURAM::getTotalPhysicalMemoryUsed()
{
    GlobalMemoryStatusEx(&memInfo);
    return static_cast<unsigned int>(memInfo.ullTotalPhys - memInfo.ullAvailPhys);
}

unsigned int WinCPURAM::getVirtualMemory()
{
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return  static_cast<unsigned int>(pmc.PrivateUsage/(1024*1024));
}

unsigned int WinCPURAM::getPhysicalMemory()
{
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return  static_cast<unsigned int>(pmc.WorkingSetSize/(1024*1024));
}

double WinCPURAM::getCPU()
{
    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;
    double percent;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));
    percent = static_cast<double>((sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart));
    percent /= (now.QuadPart - lastCPU.QuadPart);
    percent /= numProcessors;
    lastCPU = now;
    lastUserCPU = user;
    lastSysCPU = sys;

    return percent * 100;
}
