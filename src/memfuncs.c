#include "memfuncs.h"

#include "safeRW.h"
#include "taihen_extra.h"

#include <psp2kern/kernel/debug.h>

static duk_ret_t duk_read32(duk_context* ctx)
{
  uint32_t* ptr = (uint32_t*)duk_to_uint32(ctx, 0);
  uint32_t value;
  if (safeRead32(ptr, &value) < 0)
  {
    return duk_eval_error(ctx, "read32 @ %p failed", ptr);
  }
  duk_push_uint(ctx, value);
  return 1;
}

static duk_ret_t duk_read16(duk_context* ctx)
{
  uint16_t* ptr = (uint16_t*)duk_to_uint32(ctx, 0);
  uint16_t value;
  if (safeRead16(ptr, &value) < 0)
  {
    return duk_eval_error(ctx, "read16 @ %p failed", ptr);
  }
  duk_push_uint(ctx, (duk_uint_t)value);
  return 1;
}

static duk_ret_t duk_read8(duk_context* ctx)
{
  uint8_t* ptr = (uint8_t*)duk_to_uint32(ctx, 0);
  uint8_t value;
  if (safeRead8(ptr, &value) < 0)
  {
    return duk_eval_error(ctx, "read8 @ %p failed", ptr);
  }
  duk_push_uint(ctx, (duk_uint_t)value);
  return 1;
}

static duk_ret_t duk_write32(duk_context* ctx)
{
  uint32_t* ptr  = (uint32_t*)duk_to_uint32(ctx, 0);
  uint32_t value = duk_to_uint32(ctx, 1);
  if (safeWrite32(ptr, value) < 0)
  {
    return duk_eval_error(ctx, "write32(0x%08X) @ %p failed", value, ptr);
  }
  return 0;
}

static duk_ret_t duk_write16(duk_context* ctx)
{
  uint16_t* ptr  = (uint16_t*)duk_to_uint32(ctx, 0);
  uint16_t value = duk_to_uint16(ctx, 1);
  if (safeWrite16(ptr, value) < 0)
  {
    return duk_eval_error(ctx, "write16(0x%04hX) @ %p failed", value, ptr);
  }
  return 0;
}

static duk_ret_t duk_write8(duk_context* ctx)
{
  uint8_t* ptr  = (uint8_t*)duk_to_uint32(ctx, 0);
  uint8_t value = duk_to_uint16(ctx, 1) & 0xFF;
  if (safeWrite8(ptr, value) < 0)
  {
    return duk_eval_error(ctx, "write8(0x%02hhX) @ %p failed", value, ptr);
  }
  return 0;
}

static duk_ret_t duk_readbuffer(duk_context* ctx)
{
  uint32_t* src      = (uint32_t*)duk_to_uint32(ctx, 0);
  duk_size_t bufsize = 0;
  uint32_t* dst      = duk_require_buffer_data(ctx, 1, &bufsize);
  memcpy(dst, src, bufsize);
  return 0;
}

static duk_ret_t duk_writebuffer(duk_context* ctx)
{
  uint32_t* dst      = (uint32_t*)duk_to_uint32(ctx, 0);
  duk_size_t bufsize = 0;
  uint32_t* src      = duk_require_buffer_data(ctx, 1, &bufsize);
  memcpy(dst, src, bufsize);
  return 0;
}

void init_memfuncs(duk_context* ctx)
{
  duk_push_c_function(ctx, duk_read32, 1);
  duk_put_global_string(ctx, "read32");

  duk_push_c_function(ctx, duk_read16, 1);
  duk_put_global_string(ctx, "read16");

  duk_push_c_function(ctx, duk_read8, 1);
  duk_put_global_string(ctx, "read8");

  duk_push_c_function(ctx, duk_write32, 2);
  duk_put_global_string(ctx, "write32");

  duk_push_c_function(ctx, duk_write16, 2);
  duk_put_global_string(ctx, "write16");

  duk_push_c_function(ctx, duk_write8, 2);
  duk_put_global_string(ctx, "write8");

  duk_push_c_function(ctx, duk_readbuffer, 2);
  duk_put_global_string(ctx, "read32buffer");

  duk_push_c_function(ctx, duk_readbuffer, 2);
  duk_put_global_string(ctx, "read16buffer");

  duk_push_c_function(ctx, duk_readbuffer, 2);
  duk_put_global_string(ctx, "read8buffer");

  duk_push_c_function(ctx, duk_writebuffer, 2);
  duk_put_global_string(ctx, "write32buffer");

  duk_push_c_function(ctx, duk_writebuffer, 2);
  duk_put_global_string(ctx, "write16buffer");

  duk_push_c_function(ctx, duk_writebuffer, 2);
  duk_put_global_string(ctx, "write8buffer");
}
