#!/usr/bin/env bash

set -x
set -e

. .travis/activate-nodejs.sh

./ensure_dependencies.py
npm install
