# Hexmap
A code library meant for storing and rendering hexagon tilemaps for use in various applications. The original inspiration for starting the project was the desire to have a user-friendly hexagon map editor to create overworld maps for tabletop roleplaying games.


Hexmap is currently being developed on Windows and Linux (Mint/Ubuntu) with plans to support mobile platforms for hexmap viewing (but probably not editing).


There are two versions of the program:
    A basic, UI-less version
    A more traditional application made using the Qt framework


Controls
------------
Movement:  Arrow Keys
Zoom Out:  Right Shift
Zoom In:   Right Ctrl

Paint Terrain:   Left Click
Switch Terrain:  Number Keys (1 through 0)


Current Features
------------
Rendering a hexagon tilemap of an arbitrary size
    only horizontal hexagons at the moment
Painting textured hexagons from a list of loaded textures
Placing lines and curves
Basic UI done with the Qt framework


In Progress
------------
Placing objects defined in a texture atlas atlas
Improved line rendering
Hex Grid Data Import/Export


TODO
------------
Colored Splines for rivers/roads/etc
Textured Splines
Vertical Hexes
Texture Rotation (Both Hexagon Textures and Map Objects)
Android/iOS Hexmap Viewer
Hexographer Compatability
Custom Hexagon and Object Data
    ex: Descriptions, Encounters, City Info, etc




Dependancies
------------
[asdf_multiplat](https://github.com/mflagel/asdf_multiplat.git)
[Qt](https://www.qt.io/)
[Qt-Color-Widgets](https://github.com/mbasaglia/Qt-Color-Widgets.git)