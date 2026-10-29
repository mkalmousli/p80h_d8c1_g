/* SPDX-License-Identifier: MIT */
#ifndef P80H_NET_H
#define P80H_NET_H
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
int daemon_open(const char *header);
int tcp_open(const char *endpoint);
int net_send(int fd, const void *data, size_t size);
ssize_t net_read(int fd, void *data, size_t size);
int daemon_command(const char *command);
int daemon_get(const char *remote, const char *local);
int daemon_get_range(const char *remote, const char *local, uint64_t offset, uint64_t size);
int daemon_put(const char *local, const char *remote);
#endif
