/**
 * Custom bindings file skeleton
 *
 * Rename this file to "custom_bindings.c" and implement your custom bindings here.
 */
#include "../custom_bindings.h"

void add_custom_bindings(duk_context* ctx)
{
  duk_push_uint(ctx, 0xCAFECAFE);
  duk_put_global_string(ctx, "my_native_variable");
}

static duk_ret_t native_mul(duk_context* ctx)
{
  uint32_t input = duk_to_uint32(ctx, 0);
  duk_push_uint(ctx, input * 2);
  return 1;
}

struct native_fn_binding FUNCTION_BINDINGS[] = {
    /* <JS name>, <native function>, <num of args / DUK_VARARGS> */
    {"my_native_mul", native_mul, 1},
};

const int NUM_FUNCTION_BINDINGS = sizeof(FUNCTION_BINDINGS) / sizeof(FUNCTION_BINDINGS[0]);