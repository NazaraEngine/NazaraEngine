Platform | Build Status
------------ | -------------
Windows | [![Windows build status](https://github.com/NazaraEngine/NazaraEngine/actions/workflows/windows-build.yml/badge.svg?branch=main)](https://github.com/NazaraEngine/NazaraEngine/actions/workflows/windows-build.yml)
MSYS2 (MinGW64) | [![Windows build status](https://github.com/NazaraEngine/NazaraEngine/actions/workflows/msys2-build.yml/badge.svg?branch=main)](https://github.com/NazaraEngine/NazaraEngine/actions/workflows/msys2-build.yml)
Linux | [![Linux build status](https://github.com/NazaraEngine/NazaraEngine/actions/workflows/linux-build.yml/badge.svg?branch=main)](https://github.com/NazaraEngine/NazaraEngine/actions/workflows/linux-build.yml)
macOS | [![macOS build status](https://github.com/NazaraEngine/NazaraEngine/actions/workflows/macos-build.yml/badge.svg?branch=main)](https://github.com/NazaraEngine/NazaraEngine/actions/workflows/macos-build.yml)
Emscripten | [![Emscripten build status](https://github.com/NazaraEngine/NazaraEngine/actions/workflows/wasm-build.yml/badge.svg?branch=main)](https://github.com/NazaraEngine/NazaraEngine/actions/workflows/wasm-build.yml)

# Nazara Engine  

Nazara Engine is a cross-platform framework aimed at (but not limited to) real-time applications requiring audio, 2D and 3D rendering, network and more (such as video games).

Features:
- Supports Windows, Linux, macOS and the web (Android and iOS are coming).
- 2D and 3D rendering using either Vulkan, OpenGL and OpenGL ES (depending on what's available on the target system).
- Its own renderer-agnostic shader language (NZSL - Nazara Shading Language), a single modern language working with all renderer backends
- Easy to customize graphics module providing basis for advanced techniques (such as deferred shading, HDR, PBR rendering and more).
- A powerful and extensible frame graph system allowing you to easily define how your rendering works.
- Audio playback and streaming based on [OpenAL-Soft](https://www.openal-soft.org).
- 2D and 3D Physics integration (with [Chipmunk](https://chipmunk-physics.net) and [Jolt Physics]([https://github.com/MADEAPPS/newton-dynamics](https://github.com/jrouwe/JoltPhysics))).
- Optional ECS (Entity-Component-System) classes for helping with entity handling (based on the excellent [entt](https://github.com/skypjack/entt)).
- A [ENet](https://github.com/lsalzman/enet)-based reliable UDP networking protocol.

You can use it in any kind of commercial and non-commercial applications without any restriction ([MIT license](http://opensource.org/licenses/MIT)).

## Authors

Jérôme "SirLynix" Leclercq - main developper (<lynix680@gmail.com>)   

## Build and install

See [INSTALL.md](INSTALL.md) for build instructions.

## How to use

You can find tutorials on installation, compilation and use on the [official wiki](https://github.com/NazaraEngine/NazaraEngine/wiki)

## Contribute

##### Don't hesitate to contribute to Nazara Engine by:
- Extending the [wiki](https://github.com/NazaraEngine/NazaraEngine/wiki)
- Submitting a patch to GitHub  
- Post suggestions/bugs on the forum or the [GitHub tracker](https://github.com/NazaraEngine/NazaraEngine/issues)    
- [Fork the project](https://github.com/NazaraEngine/NazaraEngine/fork) on GitHub and [push your changes](https://github.com/NazaraEngine/NazaraEngine/pulls)  
- Talking about Nazara Engine to other people, spread the word!  
- Doing anything else that might help us

## Links

[Discord](https://discord.sirlynix.be/)  
[X (Twitter)](https://twitter.com/SirLynix)
[Wiki](https://github.com/NazaraEngine/NazaraEngine/wiki)  

## Thanks to:

See [THANKS.md](THANKS.md)
