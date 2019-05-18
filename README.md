# DMG emulator

Personal attempt at creating a simple DMG GameBoy emulator from scratch.

# Documentation

* [The Ultimate Game Boy talk](https://www.youtube.com/watch?v=HyzD8pNlpwI) at 33c3
* [Why did I spend 1.5 months creating a Gameboy emulator?](http://blog.rekawek.eu/2017/02/09/coffee-gb/)
* [Z80 Family CPU User Manual](http://www.myquest.nl/z80undocumented/z80cpu_um.pdf)
* [Dooskington wiki about emulators](https://github.com/Dooskington/GameLad/wiki)
* [Pan Docs](http://bgb.bircd.org/pandocs.htm)
* [Game Boy DMG original Boot ROM](http://gbdev.gg8.se/wiki/articles/Gameboy_Bootstrap_ROM)
* [Game Boy Opcodes](http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html)
* [Game Boy Opcodes (other version)](https://gamehacking.org/faqs/GameBoy_Z80_Opcode_Map.html)

# Compilation

```
make clean; make
```

Test with `./test`

# Tests

## Blarggs Cpu Instruction

|# |name               |state             |
|- |-                  |-                 |
|01|special            |:heavy_check_mark:|
|02|interrupts         |:x:               |
|03|op sp,hl           |:heavy_check_mark:|
|04|op r,imm           |:heavy_check_mark:|
|05|op rp              |:heavy_check_mark:|
|06|ld r,r             |:heavy_check_mark:|
|07|jr,jp,call,ret,rst |:heavy_check_mark:|
|08|misc instrs        |:heavy_check_mark:|
|09|op r,r             |:heavy_check_mark:|
|10|bit ops            |:heavy_check_mark:|
|11|op a,(hl)          |:heavy_check_mark:|

# TODO

* [x] Implement Z80 instruction set
* [ ] Implement interupts
* [x] Implement MMU
* [-] Implement PPU
    * [x] Draw background
    * [x] Draw window
    * [ ] Draw sprites
* [ ] Sound!
