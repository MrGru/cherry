#Cherry is created for game in C

- support OpenGL  : developing

- support Metal   : developing

- support DirectX : plan

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
