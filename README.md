# asdf_multiplat
The personal repo of Mathias Flagel: A collection of mostly-multi-platform programs and libraries


asdf_multiplat is primarily a basic C++ rendering/input/UI/utility codebase that acts as pool of common code between my various projects. Most of these side-projects are exploratory and impulse-driven. They act as an excuse to learn new and interesting things, be it new C++ features, openGL rendering techniques, or interesting areas of mathematics.

# Sub-Projects

### [Hexmap](https://github.com/mflagel/asdf_multiplat/tree/master/hexmap)
My current focus, 'Hexmap' is a program to render and edit hexagon hexagon tilemaps. My intent is to have a useful tool for creating maps for use in tabletop RPGs, but the underlying code will be useful for anything that uses tiled hexagons.

Hexmap uses Qt for a cross-platform user interface, and is currently being developed on Windows and Linux (Mint/Ubuntu)

### [Plant Generator](https://github.com/mflagel/asdf_multiplat/tree/master/PlantGenerator)
Plant Generator is a command-line utility to generate a random plant from a given JSON data file and output a human-readable paragraph describing the randomly generated plant. This project is still a work in progress, but most of the functionality is available
This small project was a good excuse to explore the new variant type in C++17, as well as try out cmake for build-configuration and Catch for unit testing.



## Dependancies
* opengl
* [sdl2](https://www.libsdl.org/index.php)
* [glm](https://github.com/g-truc/glm)
* [glew](https://github.com/nigels-com/glew)
* [SOIL](http://www.lonesock.net/soil.html)
* [cJSON](http://www.lonesock.net/soil.html)