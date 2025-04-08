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

// use primitive objects to simulate pointer
// priimitive objects are always passed to called function as pointer to it's data
var base2 = new u32();

var ret = ksceKernelGetMemblockBase(block, base2);
ksceKernelPrintf('ret= 0x%08X\n', ret);
ksceKernelPrintf('ptr= 0x%08X\n', base2.value);

// direct memory access using single values
write32(base[0], 0xDEADBABE);
ksceKernelPrintf('value= 0x%08X\n', read32(base[0]));

// direct memory access using buffers
var data = new Uint32Array(2);
data[0] = 0xCAFEBEEF;
data[1] = 0xB00BDEAD;

write32buffer(base[0], data);
var out = new Uint32Array(2);
read32buffer(base[0], out);

ksceKernelPrintf('value= 0x%08X 0x%08X\n', out[0], out[1]);
