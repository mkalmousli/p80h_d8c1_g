/* SPDX-License-Identifier: MIT */
#ifndef P80H_FEATURE_MAP_H
#define P80H_FEATURE_MAP_H
#include "graph.h"
struct inventory { struct graph graph; char dependency[64][64]; unsigned marks[64]; };
int feature_map(struct inventory *);
#endif
