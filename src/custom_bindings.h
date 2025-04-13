#ifndef __CUSTOM_BINDINGS_H__
#define __CUSTOM_BINDINGS_H__

#include "duktape/duktape.h"

struct native_fn_binding
{
  const char* name;
  duk_c_function func;
  duk_idx_t nargs;
};

extern struct native_fn_binding FUNCTION_BINDINGS[];
extern const int NUM_FUNCTION_BINDINGS;

extern void add_custom_bindings(duk_context* ctx);

#endif
