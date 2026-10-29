/* SPDX-License-Identifier: MIT */
#ifndef P80H_FEATURES_H
#define P80H_FEATURES_H
struct p80h_feature {
    const char *id, *name, *state, *hardware, *driver, *evidence, *acceptance;
};
int device_features_cli(void);
#endif
