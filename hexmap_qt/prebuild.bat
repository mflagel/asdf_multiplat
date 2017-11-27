rem del ..\include\asdfm
rem del ..\include\hexmap
rem del assets
rem del data
mklink /D ..\include\asdfm ..\asdf_multiplat\src
mklink /D ..\include\hexmap ..\hexmap\src
mklink /D assets ..\hexmap\assets
mklink /D data ..\hexmap\data