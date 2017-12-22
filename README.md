# XNU Heap Playground

macOS kext, and userspace client to play with the (`zalloc`) kernel allocator.

### Warning

This kext **severely** lowers the security of your operating system and is of **zero** use to ordinary users!  
**Do not install** unless you know what you are doing.

### Building

- kext: use the Xcode project
- client: `make all`

### Installation

Installation should be clear, obviously requires SIP to be weakened/disabled.

### License

[MIT](https://github.com/sferrini/xnu-heap-playground/blob/master/LICENSE).

### Thanks

This project uses code (`sym.{c,h}` and `common.h`) from [Siguza/hsp4](https://github.com/Siguza/hsp4). Have a look at his [repos](https://github.com/Siguza?tab=repositories)!
