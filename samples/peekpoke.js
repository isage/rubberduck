/**
 * rubberduck provides safe peek/poke primitives that operate on
 * 8-bit, 16-bit and 32-bit values while gracefully handling invalid
 * memory addresses if provided.
 * 
 * IMPORTANT: read{X}buffer/write{X}buffer are not safe primitives.
 * Providing invalid addresses to these built-in functions will
 * result in a DABT-exception-induced kernel panic.
 */
print('QUACK from js!');

// resolve funcs
var ksceKernelPrintf = resolveExport('SceSysmem', 0x88758561, 0x391B74B7);
var ksceKernelGetKblParam = resolveExport('SceSysmem', 0x3691DA45, 0x9DB56D1F);

// read from valid memory
var pKblParam = ksceKernelGetKblParam();
ksceKernelPrintf('KBL Param @ %p\n', pKblParam);
ksceKernelPrintf('pKblParam->version = 0x%04X\n', read16(pKblParam));
ksceKernelPrintf('pKblParam->size    = 0x%04X\n', read16(pKblParam + 2));

// write then read back memory
var orig = read16(pKblParam);
write16(pKblParam, 0xCAFE);
ksceKernelPrintf('Updated value:  0x%04X\n', read16(pKblParam));
write16(pKblParam, orig);
ksceKernelPrintf('Restored value: 0x%04X\n', read16(pKblParam));

// attempt to write to invalid memory (will raise error)
print('Attempting to write to NULL...');
write32(0x0, 0xDEADCAFE);
