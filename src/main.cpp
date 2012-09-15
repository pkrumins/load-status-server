#include <stdio.h>
#include <string.h>

#include "networking.h"
#include "sysinfo.h"

static char server_port[6];

char *format_response(
    const struct mem_info &mem_info,
    const struct disk_info &disk_info,
    const struct cpu_info &cpu_info
);

FILETIME last_idle;
FILETIME last_kernel;
FILETIME last_user;

#ifdef WINAPP
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char **argv8)
#endif
{

#ifdef WINAPP
    int argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
#endif

#ifdef WINAPP
    char **argv8 = (char **)malloc(sizeof(char *) * argc);
    for (int i = 0; i<argc; i++) {
        int len = wcslen(argv[i]);
        argv8[i] = (char *)malloc(sizeof(char)*(len+1));
        wcstombs(argv8[i], argv[i], len+1);
    }
#endif

    GetSystemTimes(&last_idle, &last_kernel, &last_user);

    if (argc == 2) {
        if (strcmp(argv8[1], "--help") == 0) {
            printf("usage: load-status-server.exe [port]\n");
            printf("port defaults to 7000\n");
            return 1;
        }
    }

    if (argc == 2) {
        strncpy_s(server_port, 6, argv8[1], _TRUNCATE);
    }
    else {
        strncpy_s(server_port, 6, "7000", _TRUNCATE);
    }
    
    if (!InitWinsock()) {
        return 1;
    }

    SOCKET server = CreateServerSocket(server_port);
    if (!server) {
        return 1;
    }
    
    printf("Started server on port %s\n", server_port);

    while (1) {
        SOCKET ClientSocket = accept(server, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(server);
            WSACleanup();
            return 1;
        }

        struct mem_info mem_info = get_mem_info();
        struct disk_info disk_info = get_disk_info();
        struct cpu_info cpu_info = get_cpu_info();

        char *response = format_response(mem_info, disk_info, cpu_info);
        int iSendResult = send(ClientSocket, response, strlen(response), 0);
        free(response);
        if (iSendResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            closesocket(server);
            break;
        }

        closesocket(ClientSocket);
    }
    return 0;
}

char *
format_response(
    const struct mem_info &mem_info,
    const struct disk_info &disk_info,
    const struct cpu_info &cpu_info
)
{
    char mem_usage_format[] =
        "    \"usage\": %d,\n"
        "    \"total_physical\": %lld,\n"
        "    \"free_physical\": %lld,\n"
        "    \"total_paging_file\": %lld,\n"
        "    \"free_paging_file\": %lld,\n"
        "    \"total_virtual\": %lld,\n"
        "    \"free_virtual\": %lld,\n"
        "    \"free_extended_virtual\": %lld";

    int mem_usage_len = strlen(mem_usage_format) +
        7*20 /* 20 = max length of %lld */ +
        10 /* 10 = max length of %d */ +
        1;

    char *mem_usage = (char *)malloc(sizeof(char)*mem_usage_len);
    if (!mem_usage) {
        printf("Failed allocating memory for mem_usage\n");
        exit(1);
    }
    sprintf_s(mem_usage, mem_usage_len-1, mem_usage_format,
        mem_info.usage,
        mem_info.total_physical,
        mem_info.free_physical,
        mem_info.total_paging_file,
        mem_info.free_paging_file,
        mem_info.total_virtual,
        mem_info.free_virtual,
        mem_info.free_extended_virtual);

    char disk_usage_format[] =
        "    \"free_user\": %lld,\n"
        "    \"free_total\": %lld,\n"
        "    \"total\": %lld";

    int disk_usage_len = strlen(disk_usage_format) +
        3*20 /* 20 = max length of %lld */ + 1;
    char *disk_usage = (char *)malloc(sizeof(char)*disk_usage_len);
    if (!disk_usage) {
        printf("Failed allocating memory for disk_usage\n");
        exit(1);
    }
    sprintf_s(disk_usage, disk_usage_len-1, disk_usage_format,
        disk_info.free_user,
        disk_info.free_total,
        disk_info.total);

    char cpu_usage_format[] =
        "    \"load\": %d";

    int cpu_usage_len = strlen(cpu_usage_format) + 
        10 /* max length of %d */ + 1;
    char *cpu_usage = (char *)malloc(sizeof(char)*cpu_usage_len);
    if (!cpu_usage) {
        printf("Failed allocating memory for cpu_usage\n");
        exit(1);
    }
    sprintf_s(cpu_usage, cpu_usage_len-1, cpu_usage_format,
        cpu_info.load);

    char response_format[] = 
        "{\n"
        "\"memory\": {\n"
        "%s\n"
        "},\n"
        "\"cpu\": {\n"
        "%s\n"
        "},\n"
        "\"disk\": {\n"
        "%s\n"
        "}\n"
        "}\n";

    int response_size = (
        strlen(response_format) +
        strlen(mem_usage_format) +
        strlen(disk_usage_format) +
        strlen(cpu_usage_format)
    ) * 2;
    char *response = (char *)malloc(response_size);
    sprintf_s(response, response_size-1, response_format, mem_usage, cpu_usage, disk_usage);

    free(mem_usage);
    free(disk_usage);
    free(cpu_usage);

    return response;
}
