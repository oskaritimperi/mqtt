#include "socket.h"
#include "log.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#if defined(_WIN32)
#include "win32.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#if defined(_WIN32)
static int InitializeWsa()
{
    WSADATA wsa;
    int rc;
    if ((rc = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
    {
        LOG_ERROR("WSAStartup failed: %d", rc);
        return -1;
    }
    return 0;
}

#define close closesocket
#endif

int SocketConnect(const char *host, short port)
{
    struct addrinfo hints, *servinfo, *p = NULL;
    int rv;
    char portstr[6];
    int sock;

#if defined(_WIN32)
    if (InitializeWsa() != 0)
    {
        return -1;
    }
#endif

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    assert(snprintf(portstr, sizeof(portstr), "%hu", port) < (int) sizeof(portstr));

    if ((rv = getaddrinfo(host, portstr, &hints, &servinfo)) != 0)
    {
        goto cleanup;
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

cleanup:

    if (p == NULL)
    {
#if defined(_WIN32)
        WSACleanup();
#endif
        return -1;
    }

    return sock;
}

int SocketDisconnect(int sock)
{
    int rc = close(sock);
#if defined(_WIN32)
    WSACleanup();
#endif
    return rc;
}

int64_t SocketRecv(int sock, void *buf, size_t len, int flags)
{
    return recv(sock, buf, len, flags);
}

int64_t SocketSend(int sock, const void *buf, size_t len, int flags)
{
    return send(sock, buf, len, flags);
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

int SocketSelect(int sock, int *events, int timeout)
{
    fd_set rfd, wfd;
    struct timeval tv;
    int rv;

    assert(sock != -1);
    assert(events != NULL);
    assert(*events != 0);

    FD_ZERO(&rfd);
    FD_ZERO(&wfd);

    if (*events & EV_READ)
    {
        FD_SET(sock, &rfd);
    }

    if (*events & EV_WRITE)
    {
        FD_SET(sock, &wfd);
    }

    memset(&tv, 0, sizeof(tv));
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    *events = 0;

    rv = select(sock+1, &rfd, &wfd, NULL, &tv);

    if (rv < 0)
    {
        return rv;
    }

    if (FD_ISSET(sock, &wfd))
    {
        *events = EV_WRITE;
    }

    if (FD_ISSET(sock, &rfd))
    {
        *events = EV_READ;
    }

    return rv;
}
