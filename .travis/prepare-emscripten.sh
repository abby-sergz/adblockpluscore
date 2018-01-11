#!/usr/bin/env bash

set -x
set -e

wget -qO- https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz | tar xz -C third_party/
ls third_party
pushd third_party/emsdk-portable

./emsdk update
./emsdk install emscripten-tag-1.37.3-64bit > /dev/null
./emsdk activate emscripten-tag-1.37.3-64bit

pip3 install -q --user meson
ls ~/.local/bin

popd
