#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "hexmap_qt prebuild"


echo "make deps"
pushd .
cd ../
make -j4 -f asdf_multiplat/asdf_multiplat.mk
make -j4 -f hexmap/libhexmap.mk
popd



echo "useful links"
# -s for static link
# -v for verbose
# -T to prevent ln from treating an already created link as
#    a directory to put another link into

# easier than adding the hexmap src as an include path in the settings
ln -sTv "$DIR/../asdf_multiplat/src"    "$DIR/../include/asdfm"
ln -sTv "$DIR/../hexmap/src"    "$DIR/../include/hexmap"

# allows hexmap to just search upwards form the binary to find folders
# instead of having to search down from somewhere
ln -sTv "$DIR/../hexmap/assets" "$DIR/assets"
ln -sTv "$DIR/../hexmap/data"   "$DIR/data"

exit 0