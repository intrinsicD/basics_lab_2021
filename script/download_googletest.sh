#!/usr/bin/env bash

EXTS_DIR="`dirname "$0"`/../exts"

if [[ ! (-e "$EXTS_DIR"/googletest) ]]; then
  cmake -E chdir "$EXTS_DIR" git clone --recursive https://github.com/google/googletest.git
fi