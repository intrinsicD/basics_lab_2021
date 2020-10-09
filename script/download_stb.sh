#!/usr/bin/env bash

EXTS_DIR="`dirname "$0"`/../libs/bcg_library/exts"

if [[ ! (-e "$EXTS_DIR"/stb) ]]; then
  cmake -E chdir "$EXTS_DIR" git clone --recursive https://github.com/nothings/stb.git
fi
