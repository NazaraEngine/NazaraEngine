# Installation

> **Notice**: For french speakers, a more detailed explanation of the instructions is provided here: [Instructions pour compiler](wiki/(FR)-Compiler-le-moteur).

The first step to using the engine is to recover it. This can currently be done in two ways:
1) This can be don either from the original github (only the development is continuously evolving):
```git clone https://github.com/DigitalPulseSoftware/NazaraEngine.git```
2) Or by downloading a [zip file](https://github.com/DigitalPulseSoftware/NazaraEngine/archive/master.zip) containing the entire source code of the engine.

All you have to do is go to the "build/" folder. The compilation system uses [premake](https://premake.github.io/) to generate build systems.

## Windows 
For Windows users, they can directly use ".bat" files such as: "Build_VS2017.bat". It will generate a solution "NazaraEngine.sln" ready to use that you can directly build to produce the libs or execute the samples.

## Linux

Linux users, can type: "./premake5-linux64 gmake" or with the additional argument "--cc=clang" to configure the C++ compiler used. 

Some additional libraries will be required for compilation depending on the modules used:

#### Audio module (OpenAL and libsndfile)

-   with apt:  `sudo apt-get install libopenal-dev libsndfile1-dev`
-   with pacman:  `sudo pacman -S openal libsndfile`

#### Platform module (Freetype):

-   with apt:  `sudo apt-get install libfreetype6-dev`
-   with pacman:  `sudo pacman -S freetype2`

#### Utility module (XCB + X11):

-   with apt:  `sudo apt-get install libxcb-cursor-dev libxcb-ewmh-dev libxcb-randr0-dev libxcb-icccm4-dev libxcb-keysyms1-dev libx11-dev`
-   with pacman:  `sudo pacman -S libxcb libx11`

#### OpenGL renderer module:

-   with apt:  `sudo apt-get install mesa-common-dev libgl1-mesa-dev`
-   with pacman:  `sudo pacman -S mesa`

#### Plugin Assimp:

-   with apt:  `sudo apt-get install libassimp-dev`
-   with pacman:  `sudo pacman -S assimp`

#### The one line command to install everything is:

-   with apt:  `sudo apt-get install libopenal-dev libsndfile1-dev libfreetype6-dev libxcb-cursor-dev libxcb-ewmh-dev libxcb-randr0-dev libxcb-icccm4-dev libxcb-keysyms1-dev libx11-dev mesa-common-dev libgl1-mesa-dev libassimp-dev`
-   with pacman:  `sudo pacman -S openal libsndfile freetype2 libxcb libx11 mesa assimp`

## Test

One should now be able to execute the samples provided in the folder "NazaraEngine/examples/bin/" or the unit tests within "NazaraEngine/tests/".