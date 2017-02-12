#include "socket.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#if defined(_WIN32)
#error not implemented yet
#define WIN32_MEAN_AND_LEAN 1
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

int SocketConnect(const char *host, short port)
{
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char portstr[6];
    int sock;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    assert(snprintf(portstr, sizeof(portstr), "%hu", port) < (int) sizeof(portstr));

    if ((rv = getaddrinfo(host, portstr, &hints, &servinfo)) != 0)
    {
        return -1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sock = socket(p->ai_family, p->ai_socktype,
                           p->ai_protocol)) == -1)
        {
            continue;
        }

        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sock);
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL)
    {
        return -1;
    }

    return sock;
}

int SocketDisconnect(int sock)
{
#ifdef _WIN32
    return closesocket(sock);
#else
    return close(sock);
#endif
}

int SocketSendAll(int sock, const char *buf, size_t *len)
{
    size_t total = 0;
    int rv;
    size_t remaining = *len;

    while (remaining > 0)
    {
        if ((rv = send(sock, buf+total, remaining, 0)) == -1)
        {
            break;
        }
        total += rv;
        remaining -= rv;
    }

    *len = total;

    return rv == -1 ? -1 : 0;
}
