[![Build Status](https://travis-ci.org/mflagel/asdf_multiplat.svg?branch=master)](https://travis-ci.org/mflagel/asdf_multiplat)

# asdf_multiplat
The personal repo of Mathias Flagel: A collection of mostly-multi-platform programs and libraries


asdf_multiplat is primarily a basic C++ rendering/input/UI/utility codebase that acts as pool of common code between my various projects. Most of these side-projects are exploratory and impulse-driven. They act as an excuse to learn new and interesting things, be it new C++ features, openGL rendering techniques, or interesting areas of mathematics.

The project started out in Windows using as much C++11 as MSVC allowed, but almost everything written in 2015+ was done in Linux as a way to explore and understand that environment.

Most of asdf_multiplat was written in 2011-2013, and was modified and expanded in 2015+
The difference between new and old is usually easy to spot given the shift in formatting style (and the use of C++ 11/14/17)


Sub-Projects
============
Hexmap
------------
My current focus, 'Hexmap' is a program to render and edit hexagon hexagon tilemaps. My intent is to have a useful tool for creating maps for use in tabletop RPGs, but the underlying code will be useful for anything that uses tiled hexagons.

Hexmap uses Qt for a cross-platform user interface, and is currently being developed on Windows and Linux (Mint/Ubuntu)


GURPS Track
------------
A program to create and manage characters for the Generic Universal RolePlaying System tabletop ruleset. Built using [kiUi](https://github.com/hugoam/kiui) as a user interface library, as I was growing frustrated with my own UI code in asdf_multiplat.


Projector Fun
-------------
A small sandbox to play around with rendering and geometry (meta-balls, bezier curves, 2D meshes, etc). Nothing exceptional



Dependancies
============
* opengl
* [sdl2](https://www.libsdl.org/index.php)
* [glm](https://github.com/g-truc/glm)
* [glew](https://github.com/nigels-com/glew)
* [SOIL](http://www.lonesock.net/soil.html)
* [cJSON](http://www.lonesock.net/soil.html)