print('QUACK from js!');

const SCE_KERNEL_MEMBLOCK_TYPE_KERNEL_ROOT_NC_RW = 0x10208006;

// resolve funcs
var ksceKernelPrintf = resolveExport('SceSysmem', 0x88758561, 0x391B74B7);
var ksceKernelAllocMemblock = resolveExport('SceSysmem', 0x6F25E18A, 0xC94850C9);
var ksceKernelGetMemblockBase = resolveExport('SceSysmem', 0x6F25E18A, 0xA841EDDA);

var block = ksceKernelAllocMemblock("quack", SCE_KERNEL_MEMBLOCK_TYPE_KERNEL_ROOT_NC_RW, 4096, 0);
ksceKernelPrintf('block= 0x%08X\n', block);

// use buffer objects to simulate pointer
// buffer objects are always passed to called function as pointer to it's data
// so structs can be simulated same way (for now)
var base = new Uint32Array(1);

var ret = ksceKernelGetMemblockBase(block, base);
ksceKernelPrintf('ret= 0x%08X\n', ret);
ksceKernelPrintf('ptr= 0x%08X\n', base[0]);
