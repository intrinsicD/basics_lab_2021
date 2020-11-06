#!/usr/bin/env bash

EXTS_DIR="`dirname "$0"`/../external"

if [[ ! (-e "$EXTS_DIR"/tbb) ]]; then
  cmake -E chdir "$EXTS_DIR" git clone --recursive https://github.com/wjakob/tbb.git
fi