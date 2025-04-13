# RUBBERDUCK
## js framework for touching your vita kernel

![quack](rubberduck.jpg)


## Building
1. Build and install [libffi port](https://github.com/isage/vita-packages-extra/tree/master/libffi) (`vita-makepkg && vdpm libffi-*-arm.tar.xz`)
2. `cmake -B build && cmake --build build`

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

### u32/u16/u8
`new u32(<name>)`  
Creates new typed variable/field with name <name>

`u32.prototype.readFromMemory(<addr>)`  
Reads value from memory  

`u32.prototype.writeToMemory(<addr>)`  
Writes value to memory  

#### Properties
`u32.name`  
Field name

`u32.value`  
Field value

#### Methods
`u32arr.prototype.readFromMemory(<addr>)`  
Reads and fills value from memory  

### u32arr/u16arr/u8arr
`new u32arr(<name>, <size>)`  
Creates new typed array/field with name <name> and <size> elements  

#### Properties
`u32arr.name`  
Array name

`u32arr.value`  
JS array with values

`u32arr.length`  
Size in elements

`u32arr.size`  
Size in bytes

#### Methods
`u32arr.prototype.readFromMemory(<addr>)`  
Reads and fills array from memory  

`u32arr.prototype.writeToMemory(<addr>)`  
Writes array to memory  

`u32arr.prototype.print(<offset>)`  
Dumps array values  

### Struct
`new Struct(<array of fields>)`  
Creates new structure with fields <array of fields> (see u32/u32arr, etc.)

#### Properties
`Struct.fields`  
JS array with fields

#### Methods
`Struct.prototype.readFromMemory(<addr>)`  
Reads and fills struct from memory  

`Struct.prototype.writeToMemory(<addr>)`  
Writes struct to memory  

`Struct.prototype.set(<name>,<value>)`  
Set structure field value  

`Struct.prototype.get(<name>)`  
Get structure field  

`Struct.prototype.dump()`  
Dumps struct  


## License
MIT, see LICENSE.md

## Credits

* [Cat](https://github.com/isage/)
* [CreepNT](https://github.com/CreepNT/)
* [Princess of Sleeping](https://github.com/Princess-of-Sleeping/)