[![Build Status](https://travis-ci.org/mflagel/asdf_multiplat.svg?branch=master)](https://travis-ci.org/mflagel/asdf_multiplat)

# asdf_multiplat
The personal repo of Mathias Flagel: A giant pile of somewhat multi-platform programs and libraries


The vast majority of code in this repo wasn't exactly written to be seen by anyone other than myself, and there are a great number of shortcuts and ignored warnings that could be cleaned up before this would be considered worthy of sharing. 


asdf_multiplat is mostly a basic C++ rendering/input/UI/utility codebase that acts as pool of common code between my various projects. Those various projects are pretty much the code equivilent of rambling: unfinished, exploratory, and impulse-driven.

The project started out in Windows using as much C++11 as MSVC allowed, but almost everything written in 2015 was done in Linux as a way to explore and understand that environment.

Most of asdf_multiplat was written in 2011-2013, and was modified and expanded in 2015+
The difference between new and old is usually easy to spot given the shift in formatting style (and the use of C++ 11/14/17)


Sub-Projects
============
Hexmap
------------
My current focus. This will eventually be a program to render (and eventually edit) hexagon grids. My intent is to have a decent tool for creating hexagon tile maps for use in tabletop RPGs, but the underlying code will be useful for anything that uses tiled hexagons.


GURPS Track
------------
A program to create and manage characters for the Generic Universal RolePlaying System tabletop ruleset. Built using [kiUi](https://github.com/hugoam/kiui) as a user interface library, as I was growing frustrated with my own UI code in asdf_multiplat.


Projector Fun
-------------
A small sandbox to play around with rendering and geometry (meta-balls, bezier curves, 2D meshes, etc). Nothing exceptional