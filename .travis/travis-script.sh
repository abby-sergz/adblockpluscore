#!/usr/bin/env bash

. .travis/activate-nodejs.sh
PATH=${PATH}:${TRAVIS_BUILD_DIR}/third_party/ninja:$(python3 -c "import sysconfig;print(sysconfig.get_paths('posix_user')['scripts'])")

set -x
set -e

meson --buildtype ${BUILDTYPE} build
ninja -C build
npm test
