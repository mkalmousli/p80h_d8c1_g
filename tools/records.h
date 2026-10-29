/* SPDX-License-Identifier: MIT */
#ifndef P80H_RECORDS_H
#define P80H_RECORDS_H
#include <stdio.h>
/* Five pipe-separated fields: kind|id|title|state|detail. No HTML allowed. */
int records(FILE *out, int markdown, char kind);
#endif
