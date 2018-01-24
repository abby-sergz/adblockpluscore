#!/usr/bin/env bash

. ${NVM_DIR}/nvm.sh

set -x
set -e

nvm install 8.9.4
mkdir -p third_party
bash .travis/prepare-emscripten.sh
bash .travis/prepare-ninja.sh
pip3 install -q --user meson
