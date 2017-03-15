#Cherry is created for game in C

- support OpenGL  : developing

- support Metal   : developing

- support DirectX : plan

##Note

The game folder is my working game and I can not share it so the below builds will fail.

If someone sees this coding style is interesting so you can create your own game folder and implement game.h in include/cherry/game/game.h, you can delete other files in include/cherry/game because it's only relevant to my game.

Your game will be called in application/main.c.

If you need more information then you can contact me by email <nhatdiep1991@gmail.com>

##Run

Linux
- install SDL2, freeglut
- run make.sh to build and test

IOS
- require Xcode
- require SDL2 for OpenGL branch
- run make_ios.sh to build library
- OpenGL/Metal branch will be chosen at runtime

OSX
- require Xcode
- install homebrew
- install SDL2 via homebrew
- run make.sh to build and test

ANDROID
- developing

EMSCRIPTEN
- install emscripten
- replace folder containing toolchain in make_emscripten.sh
- run make_emscripten.sh
