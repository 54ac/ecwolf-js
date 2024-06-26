## ecwolf-js

### What is this?

This is a simple WebAssembly port of [ECWolf](https://maniacsvault.net/ecwolf/), a Wolfenstein 3D source port, for use in browsers. Emulates filesystem with IndexedDB. Unused dependencies stripped away to minimize bloat, but can be copied over from the original repository if desired (see .gitignore). Uses the Emscripten JavaScript runtime. Works with the default Emscripten frontend, but [ecwolf-js-frontend](https://github.com/54ac/ecwolf-js-frontend) offers a more streamlined experience. ECWolf's netcode doesn't work - it would probably require some workarounds involving WebRTC.

### How do I play the game?

- Use a web server - anything will do, e.g. [serve](https://www.npmjs.com/package/serve).
- If you have the Emscripten SDK in your PATH, you can use emrun to serve ecwolf.html and pass command line arguments to it if you wish, e.g. _emrun build/ecwolf.html -- --nowait_.

### How do I compile it?

Download and install GCC (I used MSYS2), CMake, and the Emscripten SDK. Clone this repo - the modules are unnecessary, as Emscripten uses its own libraries for most of the dependencies. Use embuilder to build them if this doesn't happen automatically (e.g. _embuilder build sdl_). By default, CMakeLists.txt will instruct the compiler to create a debug build with the built-in frontend included. To make it work, grab the latest ecwolf.pk3 file or zip up the wadsrc/static folder yourself as ecwolf.pk3 and put it in the js-static folder. Then put your IWADs (e.g. WL6 files) in the js-static/iwad folder. If the game detects IWADs from multiple games, it will ask you to pick the game you want to launch. With all of the prerequisites added to your PATH, use the appropriate wrappers to launch CMake and make:

```
emcmake cmake -S . -B build
emmake make -C build
```
