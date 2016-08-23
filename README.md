# Sgltk example
A set of examples for the usage of the sgltk library.

## About
This is a collection of examples intended to show how to use [sgltk](http://www.github.com/pyth/sgltk).

## Building the  examples

* **Windows**

    1. Generate the makefiles or project files using `cmake`:
    
        `cmake -G` to get the list of available generators

        `cmake -G "generator_name" -DCMAKE_PREFIX_PATH="path/to/include;path/to/lib" .`
    
    2. Use `make` or open the project file in the appropriate IDE to build the project

* **Linux**

    1. If [sgltk](http://www.github.com/pyth/sgltk) is not yet installed you will have to either install it or initialize this repository's submodules. To do this use `git`:

        `git submodule init`

        `git submodule update`

    2. Generate the makefiles using `cmake`:

        `cmake .` or `cmake path/to/source`

    3. Build the examples using `make`:

        `make` or `make -j number-of-threads`

        Every example has a separate build target identically titled to the name of the folder containing that example.
