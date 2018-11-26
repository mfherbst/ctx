#!/bin/sh

VERSION="v2.4.2"
wget -c https://github.com/catchorg/Catch2/releases/download/$VERSION/catch.hpp
wget -c download_and_check https://raw.githubusercontent.com/catchorg/Catch2/$VERSION/contrib/ParseAndAddCatchTests.cmake
echo $VERSION > version
