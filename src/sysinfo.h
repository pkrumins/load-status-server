#ifndef load_status_server_sysinfo_h
#define load_status_server_sysinfo_h

extern FILETIME last_idle, last_kernel, last_user;

struct mem_info {
    int usage;
    long long total_physical;
    long long free_physical;
    long long total_paging_file;
    long long free_paging_file;
    long long total_virtual;
    long long free_virtual;
    long long free_extended_virtual;
};

/* C:\ only for now */
struct disk_info {
    long long total;
    long long free_total;
    long long free_user;
};

struct cpu_info {
    int load;
};

struct mem_info get_mem_info();
struct disk_info get_disk_info();
struct cpu_info get_cpu_info();

#endif