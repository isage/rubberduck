// import funcs
var printf = resolveExport('SceSysmem', 0x88758561, 0x391B74B7);
var sceKernelGetKblParam = resolveExport('SceSysmem', 0x3691DA45, 0x9DB56D1F);

var pKblParam = sceKernelGetKblParam();
printf("KBL Param located @ %p:\n", pKblParam);

// create struct definition
KblParam = new Struct([
    new u16("KBL Param Version"),
    new u16("KBL Param Size"),
    new u32("Current FW Version"),
    new u32("Minimum FW Version"),
    new u32("unk_C"),
    new u32("unk_10"), new u32("unk_14"), new u32("unk_18"), new u32("unk_1C"),
    new u8arr("QA Flags", 16),
    new u8arr("Boot Flags", 16),
    new u8arr("DIP Switches", 32),
    new u32("DRAM Base"),
    new u32("DRAM Size"),
    new u32("unk_68"),
    new u32("Boot Type Indicator 1"),
    new u8arr("OpenPsId", 16),
    new u32("Secure Kernel PA"),
    new u32("Secure Kernel Size"),
    new u32("context_auth_sm PA"),
    new u32("context_auth_sm Size"),
    new u32("kprx_auth_sm PA"),
    new u32("kprx_auth_sm Size"),
    new u32("prog_rvk PA"),
    new u32("prog_rvk Size"),
    new u8arr("PsCode", 8),
    new u32("Stack Cookie"),
    new u32("unk_AC_rng"),
    new u8arr("Session ID", 16),    
    new u32("Sleep Factor"),
    new u32("Wakeup Factor"),
    new u32("unk_C8"),
    new u32("Boot Controls Info"),
    new u32("suspendinfo_adr"),
    new u32("Hardware Info"),
    new u32("Power Info"),
    new u32("unk_DC"),
    new u32("unk_E0"),
    new u32("unk_E4"),
    new u8arr("HardwareInfo2", 16),
    new u32("2BL Revision"),
    new u32("Magic"),
    new u8arr("Coredump Session Key", 32)
]);

// add js method to struct
Struct.prototype.jsdump = function() {
    var offset = 0;
    this.fields.forEach(function(f) {
        if (typeof f.print == 'function') {
            f.print(offset);
        } else {
            printf("+0x%04X: %*s%0*X        %s\n", offset, 8-(f.size*2), "", f.size*2, f.value, f.name);
        }
        offset += f.size;
    });
};

// read struct from memory
KblParam.readFromMemory(pKblParam);

// native func
KblParam.dump();
// js func
KblParam.jsdump();

// override native func
Struct.prototype.dump = function() {
    print('aaaaa!');
}
KblParam.dump();
