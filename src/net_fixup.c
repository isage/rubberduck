/**
 * SceNet patch to allow sceNetRecvfrom to work in kernel mode.
 * The root cause of the bug is that the underlying routine fails to propagate
 * the "kernel mode" flag to an indirectly invoked function, which then uses
 * sceKernelCopyToUser to copy towards kernel buffer, resulting in error code
 * SCE_KERNEL_MEMORY_ACCESS_ERROR.
 *
 * Bug & patch found by Princess-Of-Sleeping.
 * Port by CreepNT
 */

#include "taihen_extra.h"

#include <psp2kern/kernel/debug.h>
#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/kernel/sysclib.h>
#include <stdint.h>
#include <taihen.h>

const uint8_t orig_code[] = {
    0xC0, 0xEF, 0x10, 0x00, // vmov.i32	d16, #0
    0x19, 0x68,             // ldr		r1, [r3]
    0xA2, 0x60,             // str		r2, [r4, #8]
    0xDA, 0xF8, 0x0C, 0x30, // ldr.w	r3, [r10, #0xC]
    0xC4, 0xE9, 0x07, 0x55, // strd r5, r5, [r4, #0x1C]
    0xA5, 0x61,             // str		r5, [r4, #0x18]
    0xE3, 0x60,             // str		r3, [r4, #0xC]
    0x61, 0x62,             // str		r1, [r4, #0x24]
    0xC4, 0xED, 0x04, 0x0B  // vstr.64	d16, [r4, #0x10]
};

const uint8_t patched_code[] = {
    0xDD, 0xF8, 0x30, 0xC0, // ldr		r12, [sp, #0x30]	!!!
    0x19, 0x68,             // ldr		r1, [r3]
    0xA2, 0x60,             // str		r2, [r4, #8]
    0xDA, 0xF8, 0x0C, 0x30, // ldr.w	r3, [r10, #0xC]
    0xC4, 0xE9, 0x07, 0xC5, // strd		r12, r5, [r4,#0x1c]	!!!
    0xA5, 0x61,             // str		r5, [r4, #0x18]
    0xE3, 0x60,             // str		r3, [r4, #0xC]
    0x61, 0x62,             // str		r1, [r4, #0x24]
    0xC4, 0xE9, 0x04, 0x55  // strd		r5, r5, [r4,#0x10]	!!!
};

SceUID g_netrecv_inject = -1;

int (*sceKernelGetModuleInfoForKernel)(SceUID pid, SceUID modid, SceKernelModuleInfo *info) = NULL;

static int init_func()
{
  if (GetExport("SceKernelModulemgr", 0xC445FA63, 0xD269F915, (uintptr_t *)&sceKernelGetModuleInfoForKernel) < 0)
  {
    if (GetExport("SceKernelModulemgr", 0x92C9FFC2, 0xDAA90093, (uintptr_t *)&sceKernelGetModuleInfoForKernel) < 0)
    {
      return -1;
    }
  }
  return 0;
}

int fixup_netrecv_bug(void)
{
  SceKernelModuleInfo netps_info;
  SceUInt32 text_segidx = 0, text_base = 0, text_size = 0;
  SceUInt32 *recvfrom_inner_addr = NULL;
  SceUID netps_id;
  SceInt32 res;

  if (init_func() < 0)
    return -1;

  netps_id = ksceKernelSearchModuleByName("SceNetPs");
  if (netps_id < 0)
  {
    ksceKernelPrintf("%s: SceNetPs not found (0x%08X)\n", __func__, netps_id);
    return netps_id;
  }

  res = sceKernelGetModuleInfoForKernel(0x10005, netps_id, &netps_info);
  if (res < 0)
  {
    ksceKernelPrintf("%s: sceKernelGetModuleInfo failed 0x%08X\n", __func__, res);
    return res;
  }
  /*
          for (int i = 0; i < 4; i++) {
                  if (netps_info.segments[i].attr & SCE_KERNEL_SEGMENT_ATTR_TEXT) {
                          text_base = netps_info.segments[i].vaddr;
                          text_size = netps_info.segments[i].memsz;
                          text_seg = i;
                          ksceKernelPrintf("%s: SceNetPs text segment found at 0x%08X size 0x%08X (%u)\n",
                                  __func__, text_base, text_size, text_seg);
                          break;
                  }
          }
  */
  text_base   = (SceUInt32)netps_info.segments[0].vaddr;
  text_size   = netps_info.segments[0].memsz;
  text_segidx = 0;
  ksceKernelPrintf("%s: SceNetPs text segment found at 0x%08X size 0x%08X (%u)\n", __func__, text_base, text_size,
                   text_segidx);

  if (text_base == 0)
  {
    ksceKernelPrintf("%s: SceNetPs text segment not found\n", __func__);
    return -1;
  }

  // Search for pattern in SceNetPs text segment
  {
    uint16_t *cur = (uint16_t *)text_base;
    uint16_t *end = (uint16_t *)(text_base + text_size - sizeof(orig_code));
    while (cur < end)
    {
      if (memcmp(cur, orig_code, sizeof(orig_code)) == 0)
      {
        recvfrom_inner_addr = (uint32_t *)cur;
        break;
      }
      cur++;
    }
  }

  if (!recvfrom_inner_addr)
  {
    ksceKernelPrintf("%s: recvfrom_inner pattern not found\n", __func__);
    return -1;
  }

  ksceKernelPrintf("%s: recvfrom_inner pattern found at %p\n", __func__, recvfrom_inner_addr);
  uint32_t offset = (uint32_t)recvfrom_inner_addr - text_base;

  g_netrecv_inject = taiInjectDataForKernel(0x10005, netps_id, text_segidx, offset, patched_code, sizeof(patched_code));
  if (g_netrecv_inject < 0)
  {
    ksceKernelPrintf("%s: taiInjectDataForKernel failed 0x%08X\n", __func__, g_netrecv_inject);
    return g_netrecv_inject;
  }

  ksceKernelPrintf("%s: taiInjectDataForKernel OK => patch applied!\n", __func__, g_netrecv_inject);
  return 0;
}