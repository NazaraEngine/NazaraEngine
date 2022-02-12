# Install

## Build

First, clone or download the project sources as a .zip

Nazara is based on [XMake](https://xmake.io), a new amazing build system which will download and compile all dependencies it won't find on your computer, even [Qt](https://www.qt.io) if you wish to build the shader nodes editor.

Once XMake is installed (note that you can also download a [portable version](https://github.com/xmake-io/xmake/releases) of XMake if you wish not to install it), you'll need to run `xmake config --mode=releasedbg` in the project folder (you can also use `--mode=debug` if you wish to build a debug version of the game).  
If you wish to include unit tests or the shader nodes editor, you'll need to add `--tests=y` and `--shadernodes=y` respectively to `xmake config` parameters.

XMake will try to find all the project dependencies on your computer and ask you to install the missing ones.

### Compile using command-line (first method)

Once you're ready to compile the engine itself, run `xmake` (or `xmake -jX` if you wish not to use all your computer threads, with X being the number of threads you wish to use) and watch as the engine compiles.

### Generate a project (second method)

XMake can also generate a project file for another tool:
- Visual Studio: `xmake project -k vs`
- CMakeLists.txt (which you can open in CLion and more): `xmake project -k cmake`
- Makefile: `xmake project -k make`
- Ninja: `xmake project -k ninja`
- XCode: `xmake project -k xcode`

You should now be able to the project file with the tool of your choice.

## Test

To run the examples, unit tests or tools, you can either run them in the project or by using the command-line:

`xmake run <exe>` with `<exe>` being the name of the executable you wish to run.

Available examples names can be found in the `examples` folder (example: `xmake run PhysicsDemo`).
You can also run unit tests with `xmake run Nazara(Client)UnitTests`.

## Install

You can also package the engine in a folder by using `xmake install -o target_folder`, on Unix-based system you can also install it system-wide by running `xmake install`.