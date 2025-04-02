print('QUACK from js!');

// resolve module function by NID
// it'll error out on eval if it's unresolved
var ksceKernelPrintf = resolveExport('SceSysmem', 0x88758561, 0x391B74B7);

// use it
ksceKernelPrintf('test %s %d\n', 'quack', 123);

// try to resolve non-existing func, shuld throw error
var unexisting = resolveExport('SceSysmem', 0xCAFEBABE, 0xDEADBEEF);
