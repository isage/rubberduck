# RUBBERDUCK
## js framework for touching your vita kernel

![quack](rubberduck.jpg)


## Building
1. Install [vitasdk-softfp](https://github.com/vitasdk-softfp)
2. Build and install [libffi port](https://github.com/isage/vita-packages-extra/tree/master/libffi) (`vita-makepkg && vdpm libffi-*-arm.tar.xz`)
3. `cmake -B build && cmake --build build`

## Usage
install skprx via taihen config (or load with any plugin loader)  
rubberduck listens for data on port `8108` (or BLOB)  
you can use netcat to send your scripts: `cat script.js | nc 192.168.1.128 8108`  
(obviously use your vita ip)

See samples in, well, samples.

## Api

### resolveExport
`ptr = resolveExport(<module name>, <libnid>, <funcnid>)`  
resolves function by module name and library/function NID. Throws on error

### read32/read16/read8
`value = read32(<addr>)`  
Reads single uint32/uint16/uint8 value from memory address

### write32/write16/write8
`write32(<addr>, <value>)`  
Writes single uint32/uint16/uint8 value to memory address

### read32buffer/read16buffer/read8buffer
`read32(<addr>,<buffer>)`  
Reads uint32/uint16/uint8 values from memory address into typed array

### write32buffer/write16buffer/write8buffer
`write32(<addr>)`  
Writes uint32/uint16/uint8 typed array values to memory address


## License
MIT, see LICENSE.md

## Credits

* [Cat](https://github.com/isage/)
* [CreepNT](https://github.com/CreepNT/)
* [Princess of Sleeping](https://github.com/Princess-of-Sleeping/)