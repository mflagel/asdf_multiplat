# Hexmap
A code library meant for storing and rendering hexagon tilemaps for use in various applications. The original inspiration for starting the project was the desire to have a user-friendly hexagon map editor to create overworld maps for tabletop roleplaying games.

Hexmap uses OpenGL for rendering. It requires at least OpenGL 3.0, but can take advantage of certain 3.3 features

Hexmap uses Qt for a cross-platform user interface, and is currently being developed on Windows and Linux (Mint/Ubuntu)


![Hexmap Screenshot](https://raw.githubusercontent.com/mflagel/asdf_multiplat/master/docs/screenshots/hexmap/hexmap_qt_colored_thick_splines.png)


Current Features
------------
* Painting textured hexagons from a list of loaded textures
* Placing colored lines and curves for rivers/roads/etc
* Basic UI using the Qt framework
* Saving and Loading
* Undo and Redo


In Progress
------------
* Placing objects defined in a texture atlas atlas
* Improved line rendering
* Extending the Qt user interface


TODO
------------
* Textured Splines
* Vertical Hexes
* Texture Rotation (Both Hexagon Textures and Map Objects)
* Android/iOS Hexmap Viewer
* Hexographer Compatability
* Custom Hexagon and Object Data (ex: Descriptions, Encounters, City Info, etc)

Dependancies
------------
* [asdf_multiplat](https://github.com/mflagel/asdf_multiplat.git)
* [Qt](https://www.qt.io/)
* [Qt-Color-Widgets](https://github.com/mbasaglia/Qt-Color-Widgets.git)