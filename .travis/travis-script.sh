#!/usr/bin/env bash

. .travis/activate-nodejs.sh

set -x
set -e


meson --buildtype ${BUILDTYPE} build
ninja -C build
npm test
