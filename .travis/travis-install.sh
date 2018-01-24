#!/usr/bin/env bash

set -x
set -e

nvm install 8.9.4
mkdir -p third_party
bash ./prepare-emscripten.sh
bash ./prepare-ninja.sh
pip3 install -q --user meson
