# Sgltk example
A set of examples for the usage of the sgltk library.

![library_test screenshot](https://github.com/pyth/screenshots/blob/master/lib_test.png)

![island screenshot](https://github.com/pyth/screenshots/blob/master/island.png)


## About
This is a collection of examples intended to show how to use [sgltk](http://www.github.com/pyth/sgltk).

## Building the  examples

    1. `conan install . --output-folder=build --build=missing`

    2. `cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release`

    3. `cmake --build build`

## Controls

* **Esc** - quit the example
* **M** - toggle mouse capture
* **W**/**A**/**S**/**D** - move the camera forward/left/backward/right
* **R**/**F** - move the camera up/down
* **Q**/**E** - roll the camera
* **Ctrl+F** - toggle fullscreen
* **L** - toggle wireframe models
* **P** - recompile shaders

Depending on the example some of these controls might not be available.
