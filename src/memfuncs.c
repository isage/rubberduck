#include "memfuncs.h"

static duk_ret_t duk_read32(duk_context *ctx)
{
    uint32_t* ptr = (uint32_t*)duk_to_uint32(ctx, 0);
    duk_push_uint(ctx, *ptr);
    return 1;
}

static duk_ret_t duk_read16(duk_context *ctx)
{
    uint16_t* ptr = (uint16_t*)duk_to_uint32(ctx, 0);
    duk_push_uint(ctx, *ptr);
    return 1;
}

static duk_ret_t duk_read8(duk_context *ctx)
{
    uint8_t* ptr = (uint8_t*)duk_to_uint32(ctx, 0);
    duk_push_uint(ctx, *ptr);
    return 1;
}

static duk_ret_t duk_write32(duk_context *ctx)
{
    uint32_t* ptr = (uint32_t*)duk_to_uint32(ctx, 0);
    uint32_t value = duk_to_uint32(ctx, 1);
    *ptr = value;
    return 0;
}

static duk_ret_t duk_write16(duk_context *ctx)
{
    uint16_t* ptr = (uint16_t*)duk_to_uint32(ctx, 0);
    uint16_t value = duk_to_uint16(ctx, 1);
    *ptr = value;
    return 0;
}

static duk_ret_t duk_write8(duk_context *ctx)
{
    uint8_t* ptr = (uint8_t*)duk_to_uint32(ctx, 0);
    uint8_t value = duk_to_uint16(ctx, 1) & 0xFF;
    *ptr = value;
    return 0;
}

void init_memfuncs(duk_context *ctx)
{

  duk_push_c_function(ctx, duk_read32, 2);
  duk_put_global_string(ctx, "read32");

  duk_push_c_function(ctx, duk_read16, 2);
  duk_put_global_string(ctx, "read16");

  duk_push_c_function(ctx, duk_read8, 2);
  duk_put_global_string(ctx, "read8");

  duk_push_c_function(ctx, duk_write32, 2);
  duk_put_global_string(ctx, "write32");

  duk_push_c_function(ctx, duk_write16, 2);
  duk_put_global_string(ctx, "write16");

  duk_push_c_function(ctx, duk_write8, 2);
  duk_put_global_string(ctx, "write8");

}
