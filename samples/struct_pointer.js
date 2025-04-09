// import funcs
var printf = resolveExport('SceSysmem', 0x88758561, 0x391B74B7);
var sceCtrlPeekBufferPositive = resolveExport('SceCtrl', 0x7823A5D1, 0xEA1D3A34);
var sceCtrlSetSamplingMode = resolveExport('SceCtrl', 0x7823A5D1, 0x80F5E418);
const SCE_CTRL_MODE_ANALOG = 1;

// create struct definition
ctrldata = new Struct([
    new u32arr("timestamp",2),
    new u32("buttons"),
    new u8("leftx"),
    new u8("lefty"),
    new u8("rightx"),
    new u8("righty"),
    new u8("up"),
    new u8("right"),
    new u8("down"),
    new u8("left"),
    new u8("lt"),
    new u8("rt"),
    new u8("l1"),
    new u8("r1"),
    new u8("triangle"),
    new u8("circle"),
    new u8("cross"),
    new u8("square"),
    new u8arr("reserved", 4),
]);

printf('struct size: 0x%08X\n',ctrldata.size);

sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

var ret = sceCtrlPeekBufferPositive(0, ctrldata, 1);
printf('ret = 0x%08X\n', ret);
ctrldata.dump();

printf("buttons: 0x%08x\n", ctrldata.get("buttons").value);
