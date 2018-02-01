#!/usr/bin/env bash

. .travis/activate-nodejs.sh

set -x
set -e

PATH=${PATH}:${TRAVIS_BUILD_DIR}/third_party/ninja:$(python3 -c "import sysconfig;print(sysconfig.get_paths('posix_user')['scripts'])")

meson --buildtype ${BUILDTYPE} build/js
ninja -C build/js
npm test

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
export CXX=clang++ CC=clang
else
export CXX=g++-7 CC=gcc-7
fi
${CXX} --version
meson -Dnative=true --buildtype ${BUILDTYPE} build/native
ninja -C build/native
./build/native/abptest

