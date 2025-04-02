#ifndef __COMPAT_H__
#define __COMPAT_H__

typedef enum SceKernelHeapAttr {
    SCE_KERNEL_HEAP_ATTR_HAS_AUTO_EXTEND = 0x00000001,
    SCE_KERNEL_HEAP_ATTR_HAS_MEMORY_TYPE = 0x00000400
} SceKernelHeapAttr;

typedef struct SceKernelHeapCreateOpt {
  SceSize size;
  SceKernelHeapAttr attr;
  SceUInt32 field_8;
  SceUInt32 field_C;
  SceUInt32 memtype;
  SceUInt32 field_14;
  SceUInt32 field_18;
} SceKernelHeapCreateOpt;


int initCompat();

#endif

