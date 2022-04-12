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


## Build Pre-requisites

FreeBSD:

    $ pkg install cmake sdl2 sdl2_image sdl2_mixer

OS X (brew):

    $ brew install cmake sdl2 sdl2_image sdl2_mixer

Ubuntu:

    $ sudo apt install cmake libsdl2-dev libsdl2-2.0-0 libjpeg-dev libwebp-dev libtiff5-dev libsdl2-image-dev libsdl2-image-2.0-0 libmikmod-dev libfishsound1-dev libsmpeg-dev liboggz2-dev libflac-dev libfluidsynth-dev libsdl2-mixer-dev libsdl2-mixer-2.0-0 libfreetype6-dev libsdl2-ttf-dev libsdl2-ttf-2.0-0

## Building and running

    $ make build run

    # or

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ ./uMario
