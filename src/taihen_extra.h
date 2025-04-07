#ifndef __TAIHEN_EXTRA_H__
#define __TAIHEN_EXTRA_H__

#include <psp2common/types.h>
#include <taihen.h>

int module_get_export_func(SceUID pid, const char* modname, uint32_t libnid, uint32_t funcnid, uintptr_t* func);
#define GetExport(modname, lib_nid, func_nid, func)                                                                    \
  module_get_export_func(KERNEL_PID, modname, lib_nid, func_nid, (uintptr_t*)func)

#endif