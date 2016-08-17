# Hexmap
A code library meant for storing and rendering hexagon tilemaps for use in various applications. The original inspiration for starting the project was the desire to have a user-friendly hexagon map editor to create overworld maps for tabletop roleplaying games.


Hexmap is currently being developed on Windows and Linux (Mint/Ubuntu) with plans to support mobile platforms for hexmap viewing (but probably not editing).


Current Features
------------
Rendering a hexagon tilemap of an arbitrary size
    only horizontal hexagons at the moment
Painting different colors onto hexagons
    currently hardcoded colors selected by number keys
Camera movement and zoom
Importing custom terrain textures
    Reads from a user-modifiable json file
Painting textures onto hexagons


In Progress
------------
Importing custom object textures
Better Sprite Atlas System


TODO
------------
Hex Grid Data Import/Export
Colored Splines for rivers/roads/etc
Textured Splines
Undo/Redo Support
Editor User Interface
    Need to figure out what UI library/system to use
Android/iOS Hexmap Viewer
Hexographer Compatability
Custom Hexagon and Object Data
    Descriptions, Encounters, City Info, etc