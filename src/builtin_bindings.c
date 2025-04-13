#include "builtin_bindings.h"
#include "custom_bindings.h"
#include "memfuncs.h"
#include "primitives.h"
#include "taihen_extra.h"

#include <ffi.h>
#include <psp2kern/kernel/debug.h>

// BINDINGS

duk_double_t dummy_get_now(void) /* dummy function to please duktape */
{
  return 0.0;
}

/* weak symbols used if no custom bindings are provided */
struct native_fn_binding __attribute__((weak)) FUNCTION_BINDINGS[] = {};
const int __attribute__((weak)) NUM_FUNCTION_BINDINGS = 0;
void __attribute__((weak)) add_custom_bindings(duk_context *ctx) { }

static duk_ret_t native_print(duk_context* ctx)
{
  duk_push_string(ctx, " ");
  duk_insert(ctx, 0);
  duk_join(ctx, duk_get_top(ctx) - 1);
  ksceKernelPrintf("%s\n", duk_to_string(ctx, -1));

  return 0;
}

static duk_ret_t duk_dlcall_wrapper(duk_context* ctx)
{
  int argn = duk_get_top(ctx);
  if (argn > 16)
  {
    return DUK_RET_EVAL_ERROR;
  }

  duk_push_this(ctx);
  duk_push_current_function(ctx);
  duk_get_prop_string(ctx, -1, "pointer");
  void* fn = duk_get_pointer(ctx, -1);

  ffi_cif cif;

  ffi_type** args = malloc(argn * sizeof(ffi_type*));
  void** values   = malloc(argn * sizeof(void*));

  ffi_arg rc;

  uint32_t intvars[16];
  char* charvars[16];
  uint8_t* buffers[16] = {0};
  uint8_t* pointer_vars[16];

  for (int i = 0; i < argn; ++i)
  {
    switch (duk_get_type(ctx, i))
    {
      case DUK_TYPE_NUMBER:
        args[i]    = &ffi_type_uint32;
        intvars[i] = duk_to_uint32(ctx, i);
        values[i]  = &(intvars[i]);
        break;
      case DUK_TYPE_POINTER:
        args[i]         = &ffi_type_pointer;
        pointer_vars[i] = duk_to_pointer(ctx, i);
        values[i]       = &(pointer_vars[i]);
        break;
      case DUK_TYPE_STRING:
        args[i]     = &ffi_type_pointer;
        charvars[i] = (char*)duk_get_string(ctx, i);
        values[i]   = &(charvars[i]);
        break;
      case DUK_TYPE_OBJECT:
        if (duk_is_buffer_data(ctx, i))
        {
          args[i]    = &ffi_type_pointer;
          intvars[i] = (uint32_t)duk_get_buffer_data(ctx, i, NULL);
          values[i]  = &(intvars[i]);
        }
        else if (duk_has_prop_string(ctx, i, "_struct"))
        {
          // get total buffer size from struct size
          duk_get_prop_string(ctx, i, "size");
          uint32_t size = duk_to_uint32(ctx, -1);
          duk_pop(ctx);
          // malloc buffer
          buffers[i] = malloc(size);

          // write structure to buffer
          duk_push_string(ctx, "writeToMemory");    // [... fields field readFromMemory]
          duk_push_uint(ctx, (uint32_t)buffers[i]); // [... fields field readFromMemory ptr]
          duk_call_prop(ctx, i, 1);                 // [... fields field ret]
          duk_pop(ctx);                             // [... fields field]

          // pass pointer to buffer
          args[i]    = &ffi_type_pointer;
          intvars[i] = (uint32_t)buffers[i];
          values[i]  = &(intvars[i]);
        }
        else if (duk_has_prop_string(ctx, i, "_primitive"))
        {
          // pass pointer to var
          args[i] = &ffi_type_pointer;

          // write value to buffer
          duk_push_string(ctx, "writeToMemory");     // [... fields field readFromMemory]
          duk_push_uint(ctx, (uint32_t)&intvars[i]); // [... fields field readFromMemory ptr]
          duk_call_prop(ctx, i, 1);                  // [... fields field ret]
          duk_pop(ctx);                              // [... fields field]

          pointer_vars[i] = (uint8_t*)&intvars[i];
          values[i]       = &(pointer_vars[i]);
        }
        break;
      default:
        free(args);
        free(values);
        // free struct buffers
        for (int i = 0; i < 16; ++i)
        {
          if (buffers[i] != NULL)
          {
            free(buffers[i]);
          }
        }
        return DUK_RET_TYPE_ERROR;
        break;
    }
  }

  if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argn, &ffi_type_sint, args) == FFI_OK)
  {
    ffi_call(&cif, fn, &rc, values);
    duk_push_int(ctx, (int32_t)rc);

    // update Pointer values
    for (int i = 0; i < argn; ++i)
    {
      switch (duk_get_type(ctx, i))
      {
        case DUK_TYPE_OBJECT:
          if (duk_is_buffer_data(ctx, i))
          {
            // we already passing a pointer, no need to update it
          }
          else if (duk_has_prop_string(ctx, i, "_struct"))
          {
            // re-read structure from buffer
            duk_push_string(ctx, "readFromMemory");   // [... fields field readFromMemory]
            duk_push_uint(ctx, (uint32_t)buffers[i]); // [... fields field readFromMemory ptr]
            duk_call_prop(ctx, i, 1);                 // [... fields field ret]
            duk_pop(ctx);                             // [... fields field]
          }
          else if (duk_has_prop_string(ctx, i, "_primitive"))
          {
            // pass pointer to var
            args[i] = &ffi_type_pointer;

            // write value to buffer
            duk_push_string(ctx, "readFromMemory");    // [... fields field readFromMemory]
            duk_push_uint(ctx, (uint32_t)&intvars[i]); // [... fields field readFromMemory ptr]
            duk_call_prop(ctx, i, 1);                  // [... fields field ret]
            duk_pop(ctx);                              // [... fields field]
          }
          break;
        default:
          break;
      }
    }

    free(args);
    free(values);
    // free struct buffers
    for (int i = 0; i < 16; ++i)
    {
      if (buffers[i] != NULL)
      {
        free(buffers[i]);
      }
    }
    return 1;
  }

  free(args);
  free(values);
  // free struct buffers
  for (int i = 0; i < 16; ++i)
  {
    if (buffers[i] != NULL)
    {
      free(buffers[i]);
    }
  }

  return DUK_RET_TYPE_ERROR;
}

static duk_ret_t duk_dlsym(duk_context* ctx)
{
  void* func = NULL;

  const char* name = duk_require_string(ctx, 0);
  uint32_t libnid  = duk_to_uint32(ctx, 1);
  uint32_t funcnid = duk_to_uint32(ctx, 2);

  int ret = GetExport(name, libnid, funcnid, &func);
  if (ret < 0)
    return duk_reference_error(ctx, "taihen says: 0x%08x", ret);

  duk_idx_t funcIndex = duk_push_c_function(ctx, duk_dlcall_wrapper, DUK_VARARGS);
  duk_push_pointer(ctx, func);
  duk_put_prop_string(ctx, funcIndex, "pointer");
  return 1;
}

void init_builtins(duk_context *ctx)
{
	duk_push_c_function(ctx, native_print, DUK_VARARGS);
	duk_put_global_string(ctx, "print");
  
	duk_push_c_function(ctx, duk_dlsym, 3);
	duk_put_global_string(ctx, "resolveExport");

	/* add built-ins defined in other files */
	init_memfuncs(ctx);
	init_primitives(ctx);

  add_custom_bindings(ctx);
  for (int i = 0; i < NUM_FUNCTION_BINDINGS; i++)
  {
    struct native_fn_binding *binding = &FUNCTION_BINDINGS[i];
    duk_push_c_function(ctx, binding->func, binding->nargs);
    duk_put_global_string(ctx, binding->name);
  }
}
