#!/usr/bin/env bash

set -x
set -e

pushd ${PROJECT_HOME}

bash .travis/travis-install.sh

# before script
pushd ./ensure_dependencies.py
npm install

bash .travis/travis-script.sh

popd

