#include <windows.h>

#include "sysinfo.h"

struct mem_info get_mem_info()
{
  MEMORYSTATUSEX x;
  x.dwLength = sizeof(x);

  GlobalMemoryStatusEx(&x);

  struct mem_info mem_info;
  mem_info.usage = x.dwMemoryLoad;
  mem_info.total_physical = x.ullTotalPhys;
  mem_info.free_physical = x.ullAvailPhys;
  mem_info.total_paging_file = x.ullTotalPageFile;
  mem_info.free_paging_file = x.ullAvailPageFile;
  mem_info.total_virtual = x.ullTotalVirtual;
  mem_info.free_virtual = x.ullAvailVirtual;
  mem_info.free_extended_virtual = x.ullAvailExtendedVirtual;

  return mem_info;
}

struct disk_info get_disk_info()
{
    struct disk_info disk_info;

    GetDiskFreeSpaceEx("C:\\",
        (PULARGE_INTEGER)&disk_info.free_user,
        (PULARGE_INTEGER)&disk_info.total,
        (PULARGE_INTEGER)&disk_info.free_total
    );
    
    return disk_info;
}

struct cpu_info get_cpu_info()
{
    struct cpu_info cpu_info;

    FILETIME idle, kernel, user;
    GetSystemTimes(&idle, &kernel, &user);

    ULARGE_INTEGER ulidle, ulkernel, uluser;
    ULARGE_INTEGER ullast_idle, ullast_kernel, ullast_user;

    ulidle.HighPart = idle.dwHighDateTime;
    ulidle.LowPart = idle.dwLowDateTime;

    ulkernel.HighPart = kernel.dwHighDateTime;
    ulkernel.LowPart = kernel.dwLowDateTime;

    uluser.HighPart = user.dwHighDateTime;
    uluser.LowPart = user.dwLowDateTime;

    ullast_idle.HighPart = last_idle.dwHighDateTime;
    ullast_idle.LowPart = last_idle.dwLowDateTime;

    ullast_kernel.HighPart = last_kernel.dwHighDateTime;
    ullast_kernel.LowPart = last_kernel.dwLowDateTime;

    ullast_user.HighPart = last_user.dwHighDateTime;
    ullast_user.LowPart = last_user.dwLowDateTime;

    ULARGE_INTEGER didle, dkernel, duser;

    didle.QuadPart = ulidle.QuadPart - ullast_idle.QuadPart;
    dkernel.QuadPart = ulkernel.QuadPart - ullast_kernel.QuadPart;
    duser.QuadPart = uluser.QuadPart - ullast_user.QuadPart;

    ULARGE_INTEGER dsystem;
    dsystem.QuadPart = dkernel.QuadPart + duser.QuadPart;

    last_idle = idle;
    last_user = user;
    last_kernel = kernel;

    if (dsystem.QuadPart == 0) {
        cpu_info.load = 0;
    }
    else {
        cpu_info.load = (int)((dsystem.QuadPart - didle.QuadPart)*100/dsystem.QuadPart);
    }

    return cpu_info;
}
