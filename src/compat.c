#include "compat.h"

#include "taihen_extra.h"

#include <psp2kern/kernel/debug.h>
#include <taihen.h>

typedef enum SceKernelHeapAttr
{
  SCE_KERNEL_HEAP_ATTR_HAS_AUTO_EXTEND = 0x00000001,
  SCE_KERNEL_HEAP_ATTR_HAS_MEMORY_TYPE = 0x00000400
} SceKernelHeapAttr;

typedef struct SceKernelHeapCreateOpt
{
  SceSize size;
  SceKernelHeapAttr attr;
  SceUInt32 field_8;
  SceUInt32 field_C;
  SceUInt32 memtype;
  SceUInt32 field_14;
  SceUInt32 field_18;
} SceKernelHeapCreateOpt;

// COMPAT

static SceUID heap = 0;

static int errno = 0;

void* (*sceKernelReallocHeapMemoryForKernel)(SceUID, void*, SceSize);
void* (*sceKernelAllocHeapMemoryForDriver)(SceUID, SceSize);
void* (*sceKernelFreeHeapMemoryForDriver)(SceUID, void*);
SceUID (*sceKernelCreateHeapForDriver)(const char* name, SceSize size, SceKernelHeapCreateOpt* pOpt);
int (*sceKernelShrinkHeapForDriver)(SceUID);

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

int shrink_heap()
{
  return sceKernelShrinkHeapForDriver(heap);
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

  if (GetExport("SceSysmem", 0x6F25E18A, 0x856FA2E3, &sceKernelShrinkHeapForDriver) < 0)
  {
    ksceKernelPrintf("QUAAAAACK?! no shrink\n");
    return -1;
  }

  // alloc heap
  SceKernelHeapCreateOpt opt;
  opt.size = sizeof(SceKernelHeapCreateOpt);
  opt.attr = SCE_KERNEL_HEAP_ATTR_HAS_AUTO_EXTEND;
  heap     = sceKernelCreateHeapForDriver("DuckHeap", 0x16000, &opt);
  ksceKernelPrintf("HEAP: 0x%08X\n", heap);

  return 0;
}
