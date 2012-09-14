This is the load status server for Windows. It runs on TCP port 7000 by
default any when you connect to this port it sends you JSON string with
CPU, Disk, and Memory usage. Useful if you want to monitor how servers
are doing.

Compile it with Visual Studio or mingw. It uses just Win32 calls.

It's compiled as console application by default. If you want it to be
a native win32 app compile it with WINAPP defined.

Example:

    One your windows server at server.com run the program:

    C:\bin\> load-status-server.exe
    Started server on port 7000

    Now if you connect to server.com:7000, it will send you JSON with
    the current server load info:

    $ nc server.com 7000
    {
    "memory": {
        "usage": 71,
        "total_physical": 1068388352,
        "free_physical": 302112768,
        "total_paging_file": 3096842240,
        "free_paging_file": 2155446272,
        "total_virtual": 2147352576,
        "free_virtual": 2130919424,
        "free_extended_virtual": 0
    },
    "cpu": {
        "load": 5
    },
    "disk": {
        "free_user": 22858027008,
        "free_total": 22858027008,
        "total": 42947571712
    }
    }

Field descriptions:
    
memory.usage - % of total physical memory used
memory.total_physical - total physical memory in bytes
memory.free_physical - free physical memory in bytes
memory.total_paging_file - total paging file size in bytes
memory.free_paging_file - free space in the paging file in bytes
memory.total_virtual - total virtual memory in bytes
memory.free_virtual - free virtual memory in bytes
memory.extended_virtual - free extended virtual memory in bytes

cpu.load - % cpu load

disk.free_user - free disk space to the user that runs the monitor in bytes
disk.free_total - total free disk space in bytes
disk.total - total disk space in bytes

