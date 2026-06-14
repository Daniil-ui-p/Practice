#ifndef RESOURCE_MONITOR_H
#define RESOURCE_MONITOR_H

#include <cstddef>
#include <ctime>

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
    
    size_t getCurrentMemoryUsage() {
#ifdef _WIN32
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        return static_cast<size_t>(memInfo.ullTotalPhys - memInfo.ullAvailPhys);
#else
        long rss = 0L;
        FILE* fp = nullptr;
        if ((fp = fopen("/proc/self/statm", "r")) == nullptr) {
            return 0;
        }
        if (fscanf(fp, "%*s%ld", &rss) != 1) {
            fclose(fp);
            return 0;
        }
        fclose(fp);
        return static_cast<size_t>(rss) * sysconf(_SC_PAGESIZE);
#endif
    }

    double getCPUTime() {
#ifdef _WIN32
        return static_cast<double>(std::clock()) / CLOCKS_PER_SEC * 1000.0;
#else
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        return (usage.ru_utime.tv_sec * 1000.0) + (usage.ru_utime.tv_usec / 1000.0);
#endif
    }

public:
    ResourceMonitor() : initial_memory(0), peak_memory(0) {
        initial_memory = getCurrentMemoryUsage();
    }
    
    void startMonitoring() {
        peak_memory = initial_memory;
    }
    
    void updatePeakMemory() {
        size_t current = getCurrentMemoryUsage();
        if (current > peak_memory) {
            peak_memory = current;
        }
    }
    
    size_t getMemoryUsage() {
        updatePeakMemory();
        return peak_memory > initial_memory ? peak_memory - initial_memory : 0;
    }
    
    double getCPULoad() {
        return getCPUTime();
    }
};

#endif // RESOURCE_MONITOR_H
