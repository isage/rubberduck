#include <psp2kern/kernel/debug.h>
#include <taihen.h>

#include "compat.h"

int module_get_export_func(SceUID pid, const char *modname, uint32_t libnid, uint32_t funcnid, uintptr_t *func);

#define GetExport(modname, lib_nid, func_nid, func)                                                               \
  module_get_export_func(KERNEL_PID, modname, lib_nid, func_nid, (uintptr_t *)func)


// COMPAT

static SceUID heap = 0;

static int errno = 0;

void* (*sceKernelReallocHeapMemoryForKernel)(SceUID, void*, SceSize);
void* (*sceKernelAllocHeapMemoryForDriver)(SceUID, SceSize);
void* (*sceKernelFreeHeapMemoryForDriver)(SceUID, void*);
SceUID (*sceKernelCreateHeapForDriver)(const char *name, SceSize size, SceKernelHeapCreateOpt *pOpt);

int* __errno(void)
{
  return &errno;
}

void* malloc(size_t size)
{
  return sceKernelAllocHeapMemoryForDriver(heap, size);
}

void free(void* ptr)
{
  sceKernelFreeHeapMemoryForDriver(heap, ptr);
}

void* realloc(void* ptr, size_t size)
{
  return sceKernelReallocHeapMemoryForKernel(heap, ptr, size);
}

void abort(void)
{
  ksceKernelPrintf("*** FATAL ERROR!!! ***\n");  
//  while(1) {}
}

int initCompat()
{
  if (GetExport("SceSysmem", 0x02451F0F, 0x8EE8B917, &sceKernelReallocHeapMemoryForKernel) < 0)
  {
    if (GetExport("SceSysmem", 0x63A519E5, 0xFDC0EA11, &sceKernelReallocHeapMemoryForKernel) < 0)
    {
        ksceKernelPrintf("QUAAAAACK?! no realloc\n");
        return -1;
    }
  }

  if (GetExport("SceSysmem", 0x6F25E18A, 0x7B4CB60A, &sceKernelAllocHeapMemoryForDriver) < 0)
  {
    ksceKernelPrintf("QUAAAAACK?! no alloc\n");
    return -1;
  }

  if (GetExport("SceSysmem", 0x6F25E18A, 0x3EBCE343, &sceKernelFreeHeapMemoryForDriver) < 0)
  {
    ksceKernelPrintf("QUAAAAACK?! no free\n");
    return -1;
  }

  if (GetExport("SceSysmem", 0x6F25E18A, 0x9328E0E8, &sceKernelCreateHeapForDriver) < 0)
  {
    ksceKernelPrintf("QUAAAAACK?! no create\n");
    return -1;
  }

  // alloc heap
  // TODO: make it extendable
  heap = sceKernelCreateHeapForDriver("DuckHeap", 0x16000, NULL);
  ksceKernelPrintf("HEAP: 0x%08X\n", heap);

  return 0;
}
