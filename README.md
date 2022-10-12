# uMario_Jakowski
uMario C++/SDL2 Game by Łukasz Jakowski

Author: Łukasz Jakowski

WWW: http://lukaszjakowski.pl
Email: jakowskidev@gmail.com

EXE and DLL - Download: http://lukaszjakowski.pl/DL/uMario.zip

YouTube video: https://www.youtube.com/watch?v=jya5He7KFsE


It is my first game made in C++.

Visual Studio 2012
SDL Tutorials which I have used:
http://lazyfoo.net/tutorials/SDL/index.php


My Google Play account: https://play.google.com/store/apps/dev?id=4635849298843013993

Web port demo: https://ferdinandsilva.com/umario


## Build Pre-requisites

FreeBSD:

    $ pkg install cmake sdl2 sdl2_image sdl2_mixer emscripten

OS X (brew):

    $ brew install cmake sdl2 sdl2_image sdl2_mixer emscripten

## Building and running

    $ make build run

    # or

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ ./uMario

## Building for web

    $ mkdir build
    $ cd build
    $ emcmake cmake ..
    $ make