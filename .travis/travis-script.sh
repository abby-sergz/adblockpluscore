#!/usr/bin/env bash

if [[ -z ${NVM_DIR} ]]; then
NVM_DIR=${HOME}/.nvm
fi

. ${NVM_DIR}/nvm.sh

set -x
set -e


meson --buildtype ${BUILDTYPE} build
ninja -C build
nvm use default
npm test
