#ifndef RESOURCE_MONITOR_H
#define RESOURCE_MONITOR_H

#include <cstddef>
#include <ctime>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#endif

class ResourceMonitor {
private:
    size_t initial_memory;
    size_t peak_memory;
    
    size_t getMemory() {
#ifdef _WIN32
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        return static_cast<size_t>(memInfo.ullTotalPhys - memInfo.ullAvailPhys);
#else
        long rss = 0L;
        FILE* fp = fopen("/proc/self/statm", "r");
        if (fp != nullptr) { 
            fscanf(fp, "%*s%ld", &rss); 
            fclose(fp); 
        }
        return static_cast<size_t>(rss) * sysconf(_SC_PAGESIZE);
#endif
    }
    
    double getCPU() {
#ifdef _WIN32
        return static_cast<double>(std::clock()) / CLOCKS_PER_SEC * 1000.0;
#else
        struct rusage u;
        getrusage(RUSAGE_SELF, &u);
        return u.ru_utime.tv_sec * 1000.0 + u.ru_utime.tv_usec / 1000.0;
#endif
    }
    
public:
    ResourceMonitor() : initial_memory(getMemory()), peak_memory(0) {}
    
    void start() { 
        peak_memory = initial_memory; 
    }
    
    size_t memoryUsed() {
        size_t cur = getMemory();
        if (cur > peak_memory) peak_memory = cur;
        return peak_memory > initial_memory ? peak_memory - initial_memory : 0;
    }
    
    double cpuTime() { 
        return getCPU(); 
    }
};

#endif
