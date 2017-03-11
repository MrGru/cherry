/*
 * Copyright (C) 2017 Manh Tran
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <cherry/server/web_server.h>
#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/server/file_descriptor.h>
#include <cherry/stdio.h>
#include <cherry/array.h>
#include <cherry/math/math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>

static void *get_in_addr(struct sockaddr *sa)
{
        if(sa->sa_family == AF_INET) {
                return &(((struct sockaddr_in *)sa)->sin_addr);
        }
        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

struct web_server *web_server_alloc()
{
        struct web_server *p    = smalloc(sizeof(struct web_server));
        p->master               = file_descriptor_set_alloc(10000);
        p->incomming            = file_descriptor_set_alloc(10000);
        p->fdmax                = 0;
        p->listener             = 0;
        p->root                 = string_alloc(0);
        return p;
}

static int __web_server_check_path(struct web_server *ws, char *path, u32 len)
{
        char c;
        int i;
        int depth = 0;
        int check_dot = 0;
        for_i(i, len) {
                c = path[i];
                switch (c) {
                        case '/':
                                if(check_dot == 2) {
                                        depth--;
                                } else {
                                        depth++;
                                }
                                check_dot = 0;
                                break;
                        case '.':
                                check_dot++;
                                break;
                        default:
                                check_dot = 0;
                                break;
                }
        }
        return depth >= 0;
}

static void __web_server_handle_msg(struct web_server *ws, u32 fd, char* msg)
{
#define MAX_DATA_LEN 1024
        struct string *path             = string_alloc(0);
        struct string *data_to_send     = string_alloc(MAX_DATA_LEN);
        char *reqline[3];
        int r, bytes_read, ok;

        reqline[0]      = strtok(msg, " \t\n");
        if(strncmp(reqline[0], "GET\0", 4) == 0) {
                reqline[1]      = strtok(NULL, " \t");
                reqline[2]      = strtok(NULL, " \t\n");
                if(strncmp(reqline[2], "HTTP/1.0", 8) != 0
                        && strncmp(reqline[2], "HTTP/1.1", 8) != 0) {
                        write(fd, "HTTP/1.0 400 Bad Request\n", 25);
                } else {
                        if(strncmp(reqline[1], "/\0", 2) == 0) {
                                reqline[1] = "/index.html";
                        }
                        string_cat_string(path, ws->root);
                        u32 len = strlen(reqline[1]);
                        string_cat(path, reqline[1], len);
                        ok = __web_server_check_path(ws, reqline[1], len);
                        if(ok) {
                                debug("send file :%s\n", path->ptr);

                                r = open(path->ptr, O_RDONLY);
                                if(r != -1) {
                                        send(fd, "HTTP/1.0 200 OK\n\n", 17, 0);
                                        while ((bytes_read = read(r, data_to_send->ptr, MAX_DATA_LEN)) > 0)
                                                write (fd, data_to_send->ptr, bytes_read);
                                        close(r);
                                } else {
                                        write(fd, "HTTP/1.0 404 Not Found\n", 23);
                                }
                        } else {
                                write(fd, "HTTP/1.0 400 Bad Request\n", 25);
                        }
                }
        }
        string_free(path);
        string_free(data_to_send);
#undef MAX_DATA_LEN
}

void web_server_start(struct web_server *ws, u16 port, char *root)
{
        ws->root->len = 0;
        string_cat(ws->root, root, strlen(root));

        struct sockaddr_storage remoteaddr;
        socklen_t addrlen;
        i32 newfd;

        char remoteIP[INET6_ADDRSTRLEN];
        int yes                 = 1;
        int rv;
        struct addrinfo hints, *ai, *p;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family         = AF_UNSPEC;
        hints.ai_socktype       = SOCK_STREAM;
        hints.ai_flags          = AI_PASSIVE;

        struct string *ports    = string_alloc(0);
        string_cat_int(ports, port);

        if((rv = getaddrinfo(NULL, ports->ptr, &hints, &ai)) != 0) {
                debug("server error: %s\n", gai_strerror(rv));
                goto finish;
        }

        for(p = ai; p != NULL; p = p->ai_next) {
                ws->listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
                if(ws->listener < 0) {
                        continue;
                }
                setsockopt(ws->listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

                if(bind(ws->listener, p->ai_addr, p->ai_addrlen) < 0) {
                        close(ws->listener);
                        continue;
                }
                break;
        }

        if(p == NULL) {
                debug("server failed to bind\n");
                goto finish;
        }

        freeaddrinfo(ai);

        if(listen(ws->listener, 10) == -1) {
                perror("listener");
                goto finish;
        }

        debug("Server started at port no. %d with root directory as %s\n",port, root);

        file_descriptor_set_add(ws->master, ws->listener);
        ws->fdmax               = ws->listener;
        struct array *actives   = array_alloc(sizeof(u32), ORDERED);

#define MAX_RECV_BUF_LEN 99999
        char *recvbuf           = smalloc(sizeof(char) * MAX_RECV_BUF_LEN);
        int nbytes              = 0;
        while(1) {
                /*
                 * listen for next incomming sockets
                 */
                file_descriptor_set_assign(ws->incomming, ws->master);
                if(select(ws->fdmax + 1, ws->incomming->set->ptr, NULL, NULL, NULL) == -1) {
                        perror("select");
                        break;
                }
                /*
                 * get active sockets
                 */
                array_force_len(actives, 0);
                file_descriptor_set_get_active(ws->incomming, actives);

                u32 *fd;
                array_for_each(fd, actives) {
                        if(*fd == ws->listener) {
                                /*
                                 * new connection
                                 */
                                addrlen = sizeof(remoteaddr);
                                newfd   = accept(ws->listener, (struct sockaddr *) &remoteaddr, &addrlen);
                                if(newfd == -1) {
                                        perror("accept\n");
                                } else {
                                        file_descriptor_set_add(ws->master, newfd);
                                        ws->fdmax = MAX(ws->fdmax, newfd);
                                        debug("new connection from %s on socket %d\n",
                                                inet_ntop(remoteaddr.ss_family,
                                                        get_in_addr((struct sockaddr *)&remoteaddr),
                                                        remoteIP, INET6_ADDRSTRLEN), newfd);
                                }
                        } else {
                                /*
                                 * receive client request
                                 */
                                nbytes = recv(*fd, recvbuf, MAX_RECV_BUF_LEN, 0);
                                if(nbytes == 0) {
                                        debug("client closed\n");
                                } else if(nbytes < 0) {
                                        debug("client error\n");
                                } else {
                                        debug("%s\n", recvbuf);
                                        __web_server_handle_msg(ws, *fd, recvbuf);

                                }
                                shutdown(*fd, SHUT_RDWR);
                                close(*fd);
                                file_descriptor_set_remove(ws->master, *fd);
                                debug("close connection\n\n");
                        }
                }
        }
finish:;
        string_free(ports);
        sfree(recvbuf);
        array_free(actives);

#undef MAX_RECV_BUF_LEN
}

void web_server_free(struct web_server *p)
{
        string_free(p->root);
        file_descriptor_set_free(p->master);
        file_descriptor_set_free(p->incomming);
        sfree(p);
}
