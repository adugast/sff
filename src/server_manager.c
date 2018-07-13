#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "server_manager.h"


static int set_fd_nonblocking(int sockfd)
{
    int flags = -1;

    flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
        return -1;

    flags |= O_NONBLOCK;

    if (fcntl(sockfd, F_SETFL, flags) == -1)
        return -1;

    return 0;
}


static int get_listening_socket(int ip_addr, int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        return -1;

    // setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, NULL, NULL); ?

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(ip_addr);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, SOMAXCONN) == -1) {
        close(sockfd);
        return -1;
    }

    return sockfd;
}


static int poll_server_loop(struct server *s)
{
    struct pollfd pfd[s->poll_max];
    pfd[0].fd = s->s_socket;
    pfd[0].events = POLLIN;
    int nfds = 1;
    int c_sock = -1;

    while (1) {

        int nread;
        int index;

        if (poll(pfd, nfds, -1) < 0) {
            perror("poll");
            return -1;
        }

        for (index = 0; index < nfds; index++) {
            if (pfd[index].revents & POLLIN) { // POLLIN: there is data to read
                if (pfd[index].fd == s->s_socket) { // if server socket triggered
                    c_sock = accept(s->s_socket, NULL, NULL);
                    if (s->accept_cb != NULL)
                        s->accept_cb(c_sock, errno);
                    if (c_sock > -1) {
                        set_fd_nonblocking(c_sock);
                        pfd[nfds].fd = c_sock;
                        pfd[nfds].events = POLLIN;
                        nfds++;
                    }
                } else {
                    // retrieve how many bytes needs to be read from the TCP socket
                    ioctl(pfd[index].fd, FIONREAD, &nread);
                    if (nread == 0) {
                        close(pfd[index].fd);
                        pfd[index].events = 0;
                        if (s->close_cb != NULL)
                            s->close_cb(pfd[index].fd);
                        int i;
                        for (i = index; i < nfds; i++)
                            pfd[index] = pfd[index + 1];
                        nfds--;
                    } else {
                        char *buffer = calloc(nread, sizeof(char));
                        if (buffer == NULL)
                            return -1;
                        read(pfd[index].fd, buffer, nread);
                        buffer[nread - 1] = '\0';
                        if (s->read_cb != NULL)
                            s->read_cb(pfd[index].fd, buffer, nread);

                        if (strcmp("e", buffer) == 0) {
                            if (s->close_cb != NULL) {
                                s->close_cb(pfd[index].fd);
                            }
                            close(s->s_socket);
                            exit(0);
                        }

                        free(buffer);
                    }
                }
            }
        }
    }

    return 0;
}


int srv_set_pollmax(struct server *s, unsigned int poll_max)
{
    // poll limitation ?
    if (poll_max > 256)
        return -1;

    s->poll_max = poll_max;

    return 0;
}


struct server *srv_init(int ip_addr, int port,
        srv_accept_cb_t accept_cb,
        srv_read_cb_t read_cb,
        srv_close_cb_t close_cb)
{
    struct server *s = calloc(1, sizeof(struct server));
    if (s == NULL)
        return NULL;

    s->s_socket = get_listening_socket(ip_addr, port);
    if (s->s_socket == -1) {
        free(s);
        return NULL;
    }

    // set server config
    s->ip_adress = ip_addr;
    s->port = port;
    s->poll_max = 32;
    // set callbacks
    s->accept_cb = accept_cb;
    s->read_cb = read_cb;
    s->close_cb = close_cb;

    return s;
}


int srv_launch(struct server *s)
{
    poll_server_loop(s);
    return 0;
}

