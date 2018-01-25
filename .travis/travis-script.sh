
#!/usr/bin/env bash

set -x
set -e


meson --buildtype ${BUILDTYPE} build
ninja -C build
nvm use default
npm test
