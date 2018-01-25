#!/usr/bin/env bash

. ${NVM_DIR}/nvm.sh

nvm install 8.9.4
nvm use 8.9.4

node --version

set -x
set -e

mkdir -p third_party
bash .travis/prepare-emscripten.sh
bash .travis/prepare-ninja.sh
pip3 install -q --user meson
