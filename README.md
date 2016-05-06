# NecroEdit

External level editor for Crypt of the NecroDancer.

**[Download the latest release here!](https://github.com/Marukyu/NecroEdit/releases)**

Please see the section *Installation* if you are having issues running NecroEdit.

## Features

* Mouse controls
* Graphical tile and object selection
* Level arrangement and duplication
* Better object placement
    * Multiple objects can be stacked on the same tile
* Movable player spawn point
* Additional tiles and objects
    * Tiles and objects that are normally inaccessible in the editor can be placed
    * Examples: Lava tiles, Golden Lute, Ninja Mask
* Additional settings for objects
    * Examples: Lord enemies, single choice items, hidden chests

## Controls

Choose a tool from the toolbar at the top of the editor window to get started.

* **Left click** to place objects or tiles.
* **Right click** to erase objects or tiles.
* **Middle click** to move the view around the level.
* Use the **mouse wheel** to zoom in or out.

## Installation

**NecroEdit requires a copy of Crypt of the NecroDancer to be installed on your computer.**

### Windows

**[Download the latest release here!](https://github.com/Marukyu/NecroEdit/releases)**

Extract the Windows release archive to your desired installation folder and open `NecroEdit.exe`.

If `vcruntime140.dll` or any other DLL file is missing, install
[Visual C++ 2015 Redistributable](https://www.microsoft.com/en-us/download/details.aspx?id=48145) restart your computer
and try running NecroEdit again.

If the DLL problem persists, please report the bug by opening an issue on this GitHub repository.

### Linux

**[Download the latest release here!](https://github.com/Marukyu/NecroEdit/releases)**

[SFML 2.3](http://sfml-dev.org), [Boost](http://boost.org) System and Boost Filesystem are required. You can install
them using your distribution's package manager or from the repsective project websites.

Extract the Linux release archive to your desired installation path and run `./NecroEdit` in the installation path.

Only 64-bit Linux binaries are provided. To use NecroEdit on 32-bit Linux, a build from source is required.

### Building from source

Clone this repository using Git and use [CMake](https://cmake.org) to build NecroEdit from source.

```sh
git clone https://github.com/Marukyu/NecroEdit.git
cd NecroEdit
cmake .
make
```

Development versions of [SFML 2.3](http://sfml-dev.org) (and its dependencies), [Boost](http://boost.org) System and
Boost Filesystem must be installed.

A C++11 compliant compiler such as GCC 4.8 or Visual Studio 2015 is required to build NecroEdit.

## Screenshots

[Editing level (zoomed in)](http://i.imgur.com/eN00kTj.png)  
[Editing level (zoomed out)](http://i.imgur.com/hej7AFZ.png)  
[Item placement](http://i.imgur.com/mBacWM8.png)  

(Game content in screenshots from Crypt of the NecroDancer; Copyright Brace Yourself Games)

## Contributors

* [Marukyu](https://github.com/Marukyu) (main development of NecroEdit)
* [Alexis Martin](https://github.com/alexismartin) (OS X compatibility)
* [RedMser](https://github.com/RedMser) (fixed a typo in the help message)

## License (MIT License)

Copyright (c) 2016 Marukyu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
