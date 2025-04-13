#include "compat.h"
#include "duktape/duktape.h"
#include "memfuncs.h"
#include "net_fixup.h"
#include "primitives.h"
#include "safeRW.h"
#include "taihen_extra.h"

#include <ffi.h>
#include <psp2kern/kernel/debug.h>
#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/netps.h>

// BINDINGS

duk_double_t dummy_get_now(void)
{
  return 0.0;
}

static void native_fatal(void* udata, const char* msg)
{
  (void)udata;
  ksceKernelPrintf("*** FATAL ERROR: %s\n", (msg ? msg : "no message"));
  abort();
}

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

static void do_duk(char* buf, uint32_t len)
{
  duk_context* ctx = duk_create_heap(NULL, NULL, NULL, NULL, native_fatal);

  // globals
  // TODO: move to init funcs?
  duk_push_c_function(ctx, native_print, DUK_VARARGS);
  duk_put_global_string(ctx, "print");

  duk_push_c_function(ctx, duk_dlsym, 3);
  duk_put_global_string(ctx, "resolveExport");

  init_memfuncs(ctx);
  init_primitives(ctx);

  // eval

  duk_int_t rc = duk_peval_lstring(ctx, buf, len);
  if (rc != 0)
  {
    ksceKernelPrintf("eval failed: %s\n", duk_safe_to_stacktrace(ctx, -1));
  }
  else
  {
    ksceKernelPrintf("result is: %s\n", duk_safe_to_string(ctx, -1));
  }

  duk_pop(ctx);
  duk_destroy_heap(ctx);
}

// network
static int g_thread_run    = 1;
static SceUID g_thread_uid = -1;

static int net_thread(SceSize args, void* argp)
{
  SceUID server_sockfd = -1;
  SceUID client_sockfd = -1;

  SceNetSockaddrIn clientaddr;
  SceNetSockaddrIn serveraddr;
  serveraddr.sin_family      = SCE_NET_AF_INET;
  serveraddr.sin_addr.s_addr = SCE_NET_INADDR_ANY;
  serveraddr.sin_port        = ksceNetHtons(8108);

  unsigned int addrlen = sizeof(SceNetSockaddrIn);
  int ret              = 0;

  while (g_thread_run)
  {
    server_sockfd = ksceNetSocket("duksock", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, 0);
    if (server_sockfd < 0)
      return 0;

    do
    {
      ksceKernelDelayThread(2 * 1000 * 1000);

      ret = ksceNetBind(server_sockfd, (SceNetSockaddr*)&serveraddr, sizeof(SceNetSockaddrIn));
      if (ret < 0)
        continue;

      ret = ksceNetListen(server_sockfd, 128);
      if (ret < 0)
        continue;
    } while (ret < 0);

    while (g_thread_run && ret >= 0)
    {
      client_sockfd = ksceNetAccept(server_sockfd, (SceNetSockaddr*)&clientaddr, &addrlen);
      if (client_sockfd < 0)
        break;

      char* buf = NULL;
      char recvbuf[1024];

      int32_t n     = 0;
      int32_t total = 0;
      do
      {
        n = ksceNetRecv(client_sockfd, recvbuf, 1024, 0);
        if (n > 0)
        {
          int32_t offset = total;
          total += n;
          buf = realloc(buf, total);
          memcpy(buf + offset, recvbuf, n);
        }
      } while (n > 0);

      ksceNetSocketClose(client_sockfd);

      do_duk(buf, total);

      free(buf);

      // shrink heap
      shrink_heap();
    }

    ksceNetSocketClose(server_sockfd);
  }

  return 0;
}

// safeRW initialization
int initSafeRW(void)
{
  int (*excpmgr_set_memaccesserror_area)(void* start, void* end);

  int res = GetExport("SceExcpmgr", 0x4CA0FDD5u, 0xC45C0D3Du, &excpmgr_set_memaccesserror_area);
  if (res < 0)
  {
    int old_res = res;
    res = GetExport("SceExcpmgr", 0x1496A5B5u, 0x44CE04B8u, &excpmgr_set_memaccesserror_area);
    if (res < 0)
    {
      ksceKernelPrintf("%s: GetExport(Old NID) failed (0x%08X)\n", __func__, old_res);
      ksceKernelPrintf("%s: GetExport(New NID) failed (0x%08X)\n", __func__, res);
      return res;
    }
  }

  res = excpmgr_set_memaccesserror_area(&_safeRWRegionStart, &_safeRWRegionEnd);
  if (res < 0)
  {
    ksceKernelPrintf("%s: excpmgr_set_memaccesserror_area() failed (0x%08X)\n", __func__, res);
    return res;
  }

  return 0;
}

// main
void _start() __attribute__((weak, alias("module_start")));
int module_start(SceSize args, void* argp)
{

  ksceKernelPrintf("QUACK!\n");

  if (fixup_netrecv_bug() < 0)
    return SCE_KERNEL_START_FAILED;
  if (initCompat() < 0)
    return SCE_KERNEL_START_FAILED;
  if (initSafeRW() < 0)
    return SCE_KERNEL_START_FAILED;

  ksceKernelPrintf("QUACK! QUACK!\n");

  g_thread_uid = ksceKernelCreateThread("quack_net_thread", net_thread, 0x3C, 0x8000, 0, 0x10000, 0);
  if (g_thread_uid < 0)
    return SCE_KERNEL_START_NO_RESIDENT;

  ksceKernelStartThread(g_thread_uid, 0, NULL);

  return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize args, void* argp)
{
  if (g_thread_uid >= 0)
  {
    g_thread_run = 0;
    ksceKernelWaitThreadEnd(g_thread_uid, NULL, NULL);
    ksceKernelDeleteThread(g_thread_uid);
  }
  return SCE_KERNEL_STOP_SUCCESS;
}
