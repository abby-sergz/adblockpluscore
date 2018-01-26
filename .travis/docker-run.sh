#!/usr/bin/env bash

set -x
set -e

pushd ${PROJECT_HOME}

bash .travis/travis-install.sh
. .travis/activate-nodejs.sh

# before script
./ensure_dependencies.py
npm install

bash .travis/travis-script.sh

popd

