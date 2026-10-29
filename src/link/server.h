/* SPDX-License-Identifier: MIT */
#ifndef P80H_SERVER_H
#define P80H_SERVER_H
#include "wire.h"
int serve_status(struct wire_message *);
int serve_exec(const struct wire_message *, struct wire_message *);
int serve_get(const struct wire_message *, struct wire_message *);
int serve_put(const struct wire_message *, struct wire_message *);
void serve_upload_close(void);
void serve_connection(int fd);
#endif
