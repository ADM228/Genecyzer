[![Build Status](https://github.com/ADM228/Genecyzer/actions/workflows/ci.yml/badge.svg)](https://github.com/ADM228/Genecyzer/actions/workflows/ci.yml)

# Genecyzer

## A tracker based on the [SNESFM](https://github.com/ADM228/SNES-FM) sound driver

Written in C++ with the [SFML framework](https://sfml-dev.org), it is (theoretically lmao) available on Windows, MacOS and Linux.  

## This specific branch is only for fast dev time

- It will only just barely work
- The progress will be ported to `master` later, but only after moving to SFML 3 and adequately finishing libriff
- The codebase of this specific branch is going to be complete spaghetti

### Features

- None  
  
### TODO

- Instruments, supporting both samples and SNES hardware noise
- Ability to make samples using modular synthesis
- Compatibility with the native version of the tracker, including the ability to open/save .srm files
- Songs, patterns, all that jazz
- SPC export, including extended ID666 tagging
- ROM export with several visualization options
- Binary song data export to use with the sound driver in your game

# Libraries' credits

## Genecyzer uses

- The wonderful [SFML framework](https://sfml-dev.org)
- The [cmake-incbin wrapper](https://github.com/morswin22/cmake-incbin) of the [incbin utility](https://github.com/graphitemaster/incbin) to include the font
- The [clip library](https://github.com/dacap/clip)
- [libriff-X](https://github.com/ADM228/libriff-X)
- [tinyfiledialogs](http://tinyfiledialogs.sourceforge.net)
- [A constexpr implementation of xxHash](https://github.com/ekpyron/xxhashct)

# FAQ

Q: Can I use it?  
A: No lmao it's nowhere near done

Q: Is this for the Sega Genesis?  
A: It will make your SNES one \:\)
