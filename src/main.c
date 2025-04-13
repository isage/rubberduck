#include "builtin_bindings.h"
#include "compat.h"
#include "duktape/duktape.h"
#include "memfuncs.h"
#include "net_fixup.h"
#include "safeRW.h"
#include "taihen_extra.h"

#include <psp2kern/kernel/debug.h>
#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/netps.h>

static void native_fatal(void* udata, const char* msg)
{
  (void)udata;
  ksceKernelPrintf("*** FATAL ERROR: %s\n", (msg ? msg : "no message"));
  abort();
}

static void do_duk(char* buf, uint32_t len)
{
  duk_context* ctx = duk_create_heap(NULL, NULL, NULL, NULL, native_fatal);

  // globals
  init_builtins(ctx);

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
    res         = GetExport("SceExcpmgr", 0x1496A5B5u, 0x44CE04B8u, &excpmgr_set_memaccesserror_area);
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
