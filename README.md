#Cherry is created for game in C

- support OpenGL  : developing

- support Metal   : developing

- support DirectX : plan

##Run

Linux
- install SDL2
- run make.sh to build and test

IOS
- require Xcode
- require SDL2 for OpenGL branch
- run make_ios.sh to build library
- developing project to choose OpenGL/Metal branch at runtime

OSX
- require Xcode
- install homebrew
- install SDL2 via homebrew
- run make.sh to build and test

ANDROID
- developing

EMSCRIPTEN
- developing
- need fix array constant index in shader
- need fix too many recursion in xml_find
