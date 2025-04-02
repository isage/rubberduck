# RUBBERDUCK
## js framework for touching your vita kernel

![quack](rubberduck.jpg)


## Building
1. Install [vitasdk-softfp](https://github.com/vitasdk-softfp)
2. Build and install [libffi port](https://github.com/isage/vita-packages-extra/tree/master/libffi) (`vita-makepkg && vdpm libffi-*-arm.tar.xz`)

## Usage
rubberduck listens for data on port `8108` (or BLOB)  
you can use netcat to send your scripts: `cat script.js | nc 192.168.1.128 8108`  
(obviously use your vita ip)

See samples in, well, samples.

## License
MIT, see LICENSE.md

## Credits

* [Cat](https://github.com/isage/)
* [CreepNT](https://github.com/CreepNT/)
* [Princess of Sleeping](https://github.com/Princess-of-Sleeping/)