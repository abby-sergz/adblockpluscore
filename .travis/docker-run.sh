#!/usr/bin/env bash

. .travis/activate-nodejs.sh

set -x
set -e

pushd ${PROJECT_HOME}

bash .travis/travis-install.sh

# before script
./ensure_dependencies.py
npm install

bash .travis/travis-script.sh

popd

