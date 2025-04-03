#ifndef __MEMFUNCS_H__
#define __MEMFUNCS_H__

#include "duktape/duktape.h"

int init_memfuncs_module(void);
void init_memfuncs(duk_context *ctx);

#endif

