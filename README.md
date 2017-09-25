# Sgltk example
A set of examples for the usage of the sgltk library.

![library_test screenshot](https://public.boxcloud.com/api/2.0/internal_files/229185037677/versions/241935226221/representations/png_paged_2048x2048/content/1.png?access_token=1!a3PcJtLKtAKyHsTz7tbj57ksOopb6VRfLaPbR5NFlJe_5l-HyfqUflQvFjUWc7Z0L0U645ejrvLRGZ4ULEJ7ZyZ-aIw7mB50MVs48mmmpa41xe-dQXsAmEyFIZfIqsk0xUpKXCQ_12gzupCWKEEr3t8lx1VVj_WAwlFJzM7kVf6b_QhgEyxMiYkYnSkOstbUChSLC0Y6ekePBIUzL9zKJeQv7NQDmeomWYyagQb0ZcDvBN5EILu9Q8SWsQ6HKpzdjgxWZACf2kz8L7DSAJLCsd7Csb30qpffagZv7Ytx5P7cXZziQTPA6bB7IH-FEthZVbnQa3-Acnblv2g1RomDZ7NZ5YegNJ5Tw9otw7_BVvJmf0aOIeGp8QnnscsrUUfxo4xREVHAcmcWULER)

![island screenshot](https://public.boxcloud.com/api/2.0/internal_files/229184965357/versions/241935148013/representations/png_paged_2048x2048/content/1.png?access_token=1!AqFJ55wCO3zgJhDeVsX7-lZIzgrw6Cg7qA-9U_B0E-BqIN6QABS-FMJR7XIdhienrCVbKK3gFsSC0sLcmFpIggYISYP_71tt00ehQMEDnGzbtF4aOOjjfbCRNImEiM2rnLL68peDhmxhalywAi8yYvpJdSmF_WSK3cs1s2y42UT5bOnEREU7RXOuiJ0RPZo826bLYg-EyEuady_Pykv__McrgdSjW4x-SE1eaeyxDR_75ucCuAg-8jvtMG9Iv6CpmquyMJKJXX9tAC69VWkahpaO6e_npw5z6OSPYQiWBGwllB_oMeopo-ApOs9Y2cQfd9z8UTZv6fvBKzKXkyNWt9fqmPyZIFCSMRzuRT2UIPI8m5ikXeLo8PbbRuXG7Vz0H2u5NMArp407rE5W)


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
