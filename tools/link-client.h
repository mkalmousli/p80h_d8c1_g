/* SPDX-License-Identifier: MIT */
#ifndef P80H_LINK_CLIENT_H
#define P80H_LINK_CLIENT_H
#include "src/link/wire.h"
struct link_session { int fd; uint32_t sequence; };
int link_open(struct link_session *);
int link_request(struct link_session *, uint16_t, unsigned char *, uint32_t,
                 struct wire_message *reply);
int link_cli(int argc, char **argv);
int link_download(struct link_session *, const char *remote, const char *local);
int link_download_resume(struct link_session *, const char *remote, const char *local);
int link_upload(struct link_session *, const char *local, const char *remote);
#endif
