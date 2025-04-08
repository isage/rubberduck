#include "primitives.h"

#include "safeRW.h"

#include <psp2kern/kernel/debug.h>

duk_ret_t duk_u32_create(duk_context* ctx)
{
  if (!duk_is_constructor_call(ctx))
  {
    return DUK_RET_TYPE_ERROR;
  }

  duk_push_this(ctx);

  duk_dup(ctx, 0);
  duk_put_prop_string(ctx, -2, "name");

  duk_push_uint(ctx, 4);
  duk_put_prop_string(ctx, -2, "size");

  duk_push_uint(ctx, 0);
  duk_put_prop_string(ctx, -2, "value");

  return 0;
}

duk_ret_t duk_u32_read(duk_context* ctx)
{
  uint32_t* ptr = (uint32_t*)duk_to_uint32(ctx, 0);
  uint32_t value;
  if (safeRead32(ptr, &value) < 0)
  {
    return duk_eval_error(ctx, "read32 @ %p failed", ptr);
  }

  duk_push_this(ctx);
  duk_push_uint(ctx, value);
  duk_put_prop_string(ctx, -2, "value");

  duk_push_uint(ctx, value);
  return 1;
}

duk_ret_t duk_u32_set(duk_context* ctx)
{
  uint32_t value = (uint32_t)duk_to_uint32(ctx, 0);
  duk_push_this(ctx);
  duk_push_uint(ctx, value);
  duk_put_prop_string(ctx, -2, "value");
  duk_push_uint(ctx, value);
  return 1;
}

duk_ret_t duk_u16_create(duk_context* ctx)
{
  if (!duk_is_constructor_call(ctx))
  {
    return DUK_RET_TYPE_ERROR;
  }

  duk_push_this(ctx);

  duk_dup(ctx, 0);
  duk_put_prop_string(ctx, -2, "name");

  duk_push_uint(ctx, 2);
  duk_put_prop_string(ctx, -2, "size");

  duk_push_uint(ctx, 0);
  duk_put_prop_string(ctx, -2, "value");

  return 0;
}

duk_ret_t duk_u16_read(duk_context* ctx)
{
  uint16_t* ptr = (uint16_t*)duk_to_uint32(ctx, 0);
  uint16_t value;
  if (safeRead16(ptr, &value) < 0)
  {
    return duk_eval_error(ctx, "read16 @ %p failed", ptr);
  }

  duk_push_this(ctx);
  duk_push_uint(ctx, value);
  duk_put_prop_string(ctx, -2, "value");

  duk_push_uint(ctx, value);
  return 1;
}

duk_ret_t duk_u16_set(duk_context* ctx)
{
  uint16_t value = (uint16_t)duk_to_uint16(ctx, 0);
  duk_push_this(ctx);
  duk_push_uint(ctx, value);
  duk_put_prop_string(ctx, -2, "value");
  duk_push_uint(ctx, value);
  return 1;
}

duk_ret_t duk_u8_create(duk_context* ctx)
{
  if (!duk_is_constructor_call(ctx))
  {
    return DUK_RET_TYPE_ERROR;
  }

  duk_push_this(ctx);

  duk_dup(ctx, 0);
  duk_put_prop_string(ctx, -2, "name");

  duk_push_uint(ctx, 1);
  duk_put_prop_string(ctx, -2, "size");

  duk_push_uint(ctx, 0);
  duk_put_prop_string(ctx, -2, "value");

  return 0;
}

duk_ret_t duk_u8_read(duk_context* ctx)
{
  uint8_t* ptr = (uint8_t*)duk_to_uint32(ctx, 0);
  uint8_t value;
  if (safeRead8(ptr, &value) < 0)
  {
    return duk_eval_error(ctx, "read8 @ %p failed", ptr);
  }

  duk_push_this(ctx);
  duk_push_uint(ctx, value);
  duk_put_prop_string(ctx, -2, "value");

  duk_push_uint(ctx, value);
  return 1;
}

duk_ret_t duk_u8_set(duk_context* ctx)
{
  uint8_t value = (uint8_t)duk_to_uint32(ctx, 0);
  duk_push_this(ctx);
  duk_push_uint(ctx, value);
  duk_put_prop_string(ctx, -2, "value");
  duk_push_uint(ctx, value);
  return 1;
}

duk_ret_t duk_u32arr_create(duk_context* ctx)
{
  if (!duk_is_constructor_call(ctx))
  {
    return DUK_RET_TYPE_ERROR;
  }

  // [name, size ]

  duk_push_this(ctx); // [name, size, this ]

  duk_dup(ctx, 0);                      // [name, size, this, name ]
  duk_put_prop_string(ctx, -2, "name"); // [name, size, this ]

  uint32_t length = duk_to_uint32(ctx, 1);

  // in bytes
  duk_push_uint(ctx, length * 4); // [name, size, this, size ]
  duk_put_prop_string(ctx, -2, "size");

  duk_push_uint(ctx, length); // [name, size, this, size ]
  duk_put_prop_string(ctx, -2, "length");

  duk_push_array(ctx); // [name, size, this, array ]
  duk_put_prop_string(ctx, -2, "value");

  return 0;
}

duk_ret_t duk_u32arr_read(duk_context* ctx)
{
  uint32_t* ptr = (uint32_t*)duk_to_uint32(ctx, 0);

  duk_push_this(ctx);                     // [ptr obj]
  duk_get_prop_string(ctx, -1, "length"); // [ptr obj length]
  uint32_t length = duk_to_uint32(ctx, -1);
  duk_pop(ctx); // [ptr obj]

  duk_get_prop_string(ctx, -1, "value"); // [ptr obj value]

  for (int i = 0; i < length; i++)
  {
    uint32_t value;
    if (safeRead32(ptr, &value) < 0)
    {
      return duk_eval_error(ctx, "read32 @ %p failed", ptr);
    }

    duk_push_uint(ctx, value);      // [ptr obj value num]
    duk_put_prop_index(ctx, -2, i); // [ptr obj value]
    ptr++;
  }

  return 0;
}

duk_ret_t duk_u32arr_print(duk_context* ctx)
{
  uint32_t offset = duk_to_uint32(ctx, 0);

  duk_push_this(ctx); // [offset obj]

  duk_get_prop_string(ctx, -1, "length"); // [offset obj length]
  uint32_t length = duk_to_uint32(ctx, -1);
  duk_pop(ctx); // [offset obj]

  duk_get_prop_string(ctx, -1, "name"); // [offset obj name]
  const char* name = duk_to_string(ctx, -1);
  duk_pop(ctx); // [offset obj]

  duk_get_prop_string(ctx, -1, "value"); // [ptr obj value]

  duk_size_t n = duk_get_length(ctx, -1);

  ksceKernelPrintf("+0x%04X:                 %s[%u]\n", offset, name, length);

  int i;
  for (i = 0; i < n; i++)
  {
    if (i % 4 == 0)
      ksceKernelPrintf("         ");
    duk_get_prop_index(ctx, -1, i); // [ptr obj value field]
    uint32_t value = (uint32_t)duk_to_uint(ctx, -1);
    duk_pop(ctx); // [ptr obj value]
    ksceKernelPrintf("%08X ", value);
    if (i % 4 == 3)
      ksceKernelPrintf("\n");
  }

  if (i % 4 != 3)
  {
    ksceKernelPrintf("\n");
  }

  return 0;
}

duk_ret_t duk_u32arr_set(duk_context* ctx)
{
  if (!duk_is_array(ctx, 0))
  {
    /* not an array */
    return 0;
  }

  duk_push_this(ctx);                     // [arr obj]
  duk_get_prop_string(ctx, -1, "length"); // [arr obj length]
  uint32_t length = duk_to_uint32(ctx, -1);
  duk_pop(ctx); // [arr obj]

  duk_size_t arrlength = duk_get_length(ctx, 0);
  duk_size_t minlen    = MIN(length, arrlength);

  duk_get_prop_string(ctx, -1, "value"); // [arr obj valuearr]

  for (int i = 0; i < minlen; i++)
  {
    duk_get_prop_index(ctx, 0, i);  // [arr obj valuearr num]
    duk_to_uint32(ctx, -1);         // [arr obj valuearr num]
    duk_put_prop_index(ctx, -2, i); // [arr obj valuearr]
  }

  return 0;
}

duk_ret_t duk_u16arr_create(duk_context* ctx)
{
  if (!duk_is_constructor_call(ctx))
  {
    return DUK_RET_TYPE_ERROR;
  }

  // [name, size ]

  duk_push_this(ctx); // [name, size, this ]

  duk_dup(ctx, 0);                      // [name, size, this, name ]
  duk_put_prop_string(ctx, -2, "name"); // [name, size, this ]

  uint32_t length = duk_to_uint32(ctx, 1);

  // in bytes
  duk_push_uint(ctx, length * 2); // [name, size, this, size ]
  duk_put_prop_string(ctx, -2, "size");

  duk_push_uint(ctx, length); // [name, size, this, size ]
  duk_put_prop_string(ctx, -2, "length");

  duk_push_array(ctx); // [name, size, this, array ]
  duk_put_prop_string(ctx, -2, "value");

  return 0;
}

duk_ret_t duk_u16arr_read(duk_context* ctx)
{
  uint16_t* ptr = (uint16_t*)duk_to_uint32(ctx, 0);

  duk_push_this(ctx);                     // [ptr obj]
  duk_get_prop_string(ctx, -1, "length"); // [ptr obj length]
  uint32_t length = duk_to_uint32(ctx, -1);
  duk_pop(ctx); // [ptr obj]

  duk_get_prop_string(ctx, -1, "value"); // [ptr obj value]

  for (int i = 0; i < length; i++)
  {
    uint16_t value;
    if (safeRead16(ptr, &value) < 0)
    {
      return duk_eval_error(ctx, "read16 @ %p failed", ptr);
    }

    duk_push_uint(ctx, value);      // [ptr obj value num]
    duk_put_prop_index(ctx, -2, i); // [ptr obj value]
    ptr++;
  }

  return 0;
}

duk_ret_t duk_u16arr_print(duk_context* ctx)
{
  uint32_t offset = duk_to_uint32(ctx, 0);

  duk_push_this(ctx); // [offset obj]

  duk_get_prop_string(ctx, -1, "length"); // [offset obj length]
  uint32_t length = duk_to_uint32(ctx, -1);
  duk_pop(ctx); // [offset obj]

  duk_get_prop_string(ctx, -1, "name"); // [offset obj name]
  const char* name = duk_to_string(ctx, -1);
  duk_pop(ctx); // [offset obj]

  duk_get_prop_string(ctx, -1, "value"); // [ptr obj value]

  duk_size_t n = duk_get_length(ctx, -1);

  ksceKernelPrintf("+0x%04X:                 %s[%u]\n", offset, name, length);

  int i;
  for (i = 0; i < n; i++)
  {
    if (i % 8 == 0)
      ksceKernelPrintf("         ");
    duk_get_prop_index(ctx, -1, i); // [ptr obj value field]
    uint16_t value = (uint16_t)duk_to_uint(ctx, -1);
    duk_pop(ctx); // [ptr obj value]
    ksceKernelPrintf("%04X ", value);
    if (i % 8 == 7)
      ksceKernelPrintf("\n");
  }

  if (i % 8 != 7)
  {
    ksceKernelPrintf("\n");
  }

  return 0;
}

duk_ret_t duk_u16arr_set(duk_context* ctx)
{
  if (!duk_is_array(ctx, 0))
  {
    /* not an array */
    return 0;
  }

  duk_push_this(ctx);                     // [arr obj]
  duk_get_prop_string(ctx, -1, "length"); // [arr obj length]
  uint32_t length = duk_to_uint32(ctx, -1);
  duk_pop(ctx); // [arr obj]

  duk_size_t arrlength = duk_get_length(ctx, 0);
  duk_size_t minlen    = MIN(length, arrlength);

  duk_get_prop_string(ctx, -1, "value"); // [arr obj valuearr]

  for (int i = 0; i < minlen; i++)
  {
    duk_get_prop_index(ctx, 0, i);  // [arr obj valuearr num]
    duk_to_uint16(ctx, -1);         // [arr obj valuearr num]
    duk_put_prop_index(ctx, -2, i); // [arr obj valuearr]
  }

  return 0;
}

duk_ret_t duk_u8arr_create(duk_context* ctx)
{
  if (!duk_is_constructor_call(ctx))
  {
    return DUK_RET_TYPE_ERROR;
  }

  // [name, size ]

  duk_push_this(ctx); // [name, size, this ]

  duk_dup(ctx, 0);                      // [name, size, this, name ]
  duk_put_prop_string(ctx, -2, "name"); // [name, size, this ]

  uint32_t length = duk_to_uint32(ctx, 1);

  // in bytes
  duk_push_uint(ctx, length * 1); // [name, size, this, size ]
  duk_put_prop_string(ctx, -2, "size");

  duk_push_uint(ctx, length); // [name, size, this, size ]
  duk_put_prop_string(ctx, -2, "length");

  duk_push_array(ctx); // [name, size, this, array ]
  duk_put_prop_string(ctx, -2, "value");

  return 0;
}

duk_ret_t duk_u8arr_read(duk_context* ctx)
{
  uint8_t* ptr = (uint8_t*)duk_to_uint32(ctx, 0);

  duk_push_this(ctx);                     // [ptr obj]
  duk_get_prop_string(ctx, -1, "length"); // [ptr obj length]
  uint32_t length = duk_to_uint32(ctx, -1);
  duk_pop(ctx); // [ptr obj]

  duk_get_prop_string(ctx, -1, "value"); // [ptr obj value]

  for (int i = 0; i < length; i++)
  {
    uint8_t value;
    if (safeRead8(ptr, &value) < 0)
    {
      return duk_eval_error(ctx, "read8 @ %p failed", ptr);
    }

    duk_push_uint(ctx, value);      // [ptr obj value num]
    duk_put_prop_index(ctx, -2, i); // [ptr obj value]
    ptr++;
  }

  return 0;
}

duk_ret_t duk_u8arr_print(duk_context* ctx)
{
  uint32_t offset = duk_to_uint32(ctx, 0);

  duk_push_this(ctx); // [offset obj]

  duk_get_prop_string(ctx, -1, "length"); // [offset obj length]
  uint32_t length = duk_to_uint32(ctx, -1);
  duk_pop(ctx); // [offset obj]

  duk_get_prop_string(ctx, -1, "name"); // [offset obj name]
  const char* name = duk_to_string(ctx, -1);
  duk_pop(ctx); // [offset obj]

  duk_get_prop_string(ctx, -1, "value"); // [ptr obj value]

  duk_size_t n = duk_get_length(ctx, -1);

  ksceKernelPrintf("+0x%04X:                 %s[%u]\n", offset, name, length);

  int i;
  for (i = 0; i < n; i++)
  {
    if (i % 16 == 0)
      ksceKernelPrintf("         ");
    duk_get_prop_index(ctx, -1, i); // [ptr obj value field]
    uint8_t value = (uint8_t)duk_to_uint(ctx, -1);
    duk_pop(ctx); // [ptr obj value]
    ksceKernelPrintf("%02X ", value);
    if (i % 16 == 15)
      ksceKernelPrintf("\n");
  }

  if (i % 16 != 15)
  {
    ksceKernelPrintf("\n");
  }

  return 0;
}

duk_ret_t duk_u8arr_set(duk_context* ctx)
{
  if (!duk_is_array(ctx, 0))
  {
    /* not an array */
    return 0;
  }

  duk_push_this(ctx);                     // [arr obj]
  duk_get_prop_string(ctx, -1, "length"); // [arr obj length]
  uint32_t length = duk_to_uint32(ctx, -1);
  duk_pop(ctx); // [arr obj]

  duk_size_t arrlength = duk_get_length(ctx, 0);
  duk_size_t minlen    = MIN(length, arrlength);

  duk_get_prop_string(ctx, -1, "value"); // [arr obj valuearr]

  for (int i = 0; i < minlen; i++)
  {
    duk_get_prop_index(ctx, 0, i);                 // [arr obj valuearr num]
    uint8_t val = (uint8_t)duk_to_uint16(ctx, -1); // [arr obj valuearr num]
    duk_pop(ctx);                                  // [arr obj valuearr]
    duk_push_uint(ctx, val);                       // [arr obj valuearr num]
    duk_put_prop_index(ctx, -2, i);                // [arr obj valuearr]
  }

  return 0;
}

duk_ret_t duk_struct_create(duk_context* ctx)
{
  if (!duk_is_constructor_call(ctx))
  {
    return DUK_RET_TYPE_ERROR;
  }

  duk_push_this(ctx); // [arr this]

  duk_dup(ctx, 0);                        // [arr this arr]
  duk_put_prop_string(ctx, -2, "fields"); // [arr this]

  return 0;
}

duk_ret_t duk_struct_read(duk_context* ctx)
{
  uint8_t* ptr = (uint8_t*)duk_to_uint32(ctx, 0);

  duk_push_this(ctx);
  duk_get_prop_string(ctx, -1, "fields"); // [... fields]
  duk_size_t n = duk_get_length(ctx, -1);
  for (duk_size_t i = 0; i < n; i++)
  {
    duk_get_prop_index(ctx, -1, i); // [... fields field]

    duk_get_prop_string(ctx, -1, "size"); // [... fields field size]
    uint32_t size = duk_to_uint32(ctx, -1);
    duk_pop(ctx); // [... fields field]

    duk_push_string(ctx, "readFromMemory"); // [... fields field readFromMemory]
    duk_push_uint(ctx, (uint32_t)ptr);      // [... fields field readFromMemory ptr]
    duk_call_prop(ctx, -3, 1);              // [... fields field ret]
    duk_pop_2(ctx);                         // [... fields]
    ptr += size;
  }
  return 0;
}

duk_ret_t duk_struct_dump(duk_context* ctx)
{
  duk_push_this(ctx);
  duk_get_prop_string(ctx, -1, "fields"); // [... fields]
  duk_size_t n = duk_get_length(ctx, -1);

  uint32_t offset = 0;

  for (duk_size_t i = 0; i < n; i++)
  {
    duk_get_prop_index(ctx, -1, i); // [... fields field]

    duk_get_prop_string(ctx, -1, "size"); // [... fields field size]
    uint32_t size = duk_to_uint32(ctx, -1);
    duk_pop(ctx); // [... fields field]

    duk_get_prop_string(ctx, -1, "name"); // [... fields field name]
    const char* name = duk_to_string(ctx, -1);
    duk_pop(ctx); // [... fields field]

    uint32_t value = 0;

    duk_get_prop_string(ctx, -1, "value"); // [... fields field value]
    // buffers value is array
    if (!duk_is_array(ctx, -1))
    {
      value = duk_to_uint32(ctx, -1);
    }
    duk_pop(ctx); // [... fields field]

    // if we have print in obj - use it, else use default
    if (duk_has_prop_string(ctx, -1, "print"))
    {
      duk_push_string(ctx, "print"); // [... fields field print]
      duk_push_uint(ctx, offset);    // [... fields field print offset]
      duk_call_prop(ctx, -3, 1);     // [... fields field ret]
      duk_pop(ctx);                  // [... fields field]
    }
    else
    {
      ksceKernelPrintf("+0x%04X: %*s%0*X        %s\n", offset, 8 - (size * 2), "", size * 2, value, name);
    }

    duk_pop(ctx); // [... fields]

    offset += size;
  }
  return 0;
}

duk_ret_t duk_struct_set(duk_context* ctx)
{
  const char* nameset = duk_to_string(ctx, 0);
  duk_push_this(ctx);
  duk_get_prop_string(ctx, -1, "fields"); // [... fields]
  duk_size_t n = duk_get_length(ctx, -1);

  for (duk_size_t i = 0; i < n; i++)
  {
    duk_get_prop_index(ctx, -1, i); // [... fields field]

    duk_get_prop_string(ctx, -1, "name"); // [... fields field name]
    const char* name = duk_to_string(ctx, -1);
    duk_pop(ctx); // [... fields field]

    if (strcmp(nameset, name) == 0)
    {
      if (duk_has_prop_string(ctx, -1, "set"))
      {
        duk_push_string(ctx, "set"); // [... fields field set]
        duk_dup(ctx, 1);             // [... fields field set value]
        duk_call_prop(ctx, -3, 1);   // [... fields field ret]
        duk_pop(ctx);                // [... fields field]
      }
    }

    duk_pop(ctx); // [... fields]
  }
  return 0;
}

void init_primitives(duk_context* ctx)
{
  duk_push_c_function(ctx, duk_u32_create, 1);
  duk_push_object(ctx);
  duk_push_c_function(ctx, duk_u32_read, 1);
  duk_put_prop_string(ctx, -2, "readFromMemory");
  duk_push_c_function(ctx, duk_u32_set, 1);
  duk_put_prop_string(ctx, -2, "set");
  duk_put_prop_string(ctx, -2, "prototype");
  duk_put_global_string(ctx, "u32");

  duk_push_c_function(ctx, duk_u16_create, 1);
  duk_push_object(ctx);
  duk_push_c_function(ctx, duk_u16_read, 1);
  duk_put_prop_string(ctx, -2, "readFromMemory");
  duk_push_c_function(ctx, duk_u16_set, 1);
  duk_put_prop_string(ctx, -2, "set");
  duk_put_prop_string(ctx, -2, "prototype");
  duk_put_global_string(ctx, "u16");

  duk_push_c_function(ctx, duk_u8_create, 1);
  duk_push_object(ctx);
  duk_push_c_function(ctx, duk_u8_read, 1);
  duk_put_prop_string(ctx, -2, "readFromMemory");
  duk_push_c_function(ctx, duk_u8_set, 1);
  duk_put_prop_string(ctx, -2, "set");
  duk_put_prop_string(ctx, -2, "prototype");
  duk_put_global_string(ctx, "u8");

  duk_push_c_function(ctx, duk_u32arr_create, 2);
  duk_push_object(ctx);
  duk_push_c_function(ctx, duk_u32arr_read, 1);
  duk_put_prop_string(ctx, -2, "readFromMemory");
  duk_push_c_function(ctx, duk_u32arr_print, 1);
  duk_put_prop_string(ctx, -2, "print");
  duk_push_c_function(ctx, duk_u32arr_set, 1);
  duk_put_prop_string(ctx, -2, "set");
  duk_put_prop_string(ctx, -2, "prototype");
  duk_put_global_string(ctx, "u32arr");

  duk_push_c_function(ctx, duk_u16arr_create, 2);
  duk_push_object(ctx);
  duk_push_c_function(ctx, duk_u16arr_read, 1);
  duk_put_prop_string(ctx, -2, "readFromMemory");
  duk_push_c_function(ctx, duk_u16arr_print, 1);
  duk_put_prop_string(ctx, -2, "print");
  duk_put_prop_string(ctx, -2, "prototype");
  duk_put_global_string(ctx, "u16arr");

  duk_push_c_function(ctx, duk_u8arr_create, 2);
  duk_push_object(ctx);
  duk_push_c_function(ctx, duk_u8arr_read, 1);
  duk_put_prop_string(ctx, -2, "readFromMemory");
  duk_push_c_function(ctx, duk_u8arr_print, 1);
  duk_put_prop_string(ctx, -2, "print");
  duk_put_prop_string(ctx, -2, "prototype");
  duk_put_global_string(ctx, "u8arr");

  duk_push_c_function(ctx, duk_struct_create, 1);
  duk_push_object(ctx);
  duk_push_c_function(ctx, duk_struct_read, 1);
  duk_put_prop_string(ctx, -2, "readFromMemory");
  duk_push_c_function(ctx, duk_struct_dump, 0);
  duk_put_prop_string(ctx, -2, "dump");
  duk_push_c_function(ctx, duk_struct_set, 2);
  duk_put_prop_string(ctx, -2, "set");
  duk_put_prop_string(ctx, -2, "prototype");
  duk_put_global_string(ctx, "Struct");
}
