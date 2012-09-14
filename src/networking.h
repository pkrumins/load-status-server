#ifndef load_status_server_networking_h
#define load_status_server_networking_h

#include <winsock2.h>
#include <ws2tcpip.h>

extern bool InitWinsock();
extern SOCKET CreateServerSocket(const char *port);

#endif
