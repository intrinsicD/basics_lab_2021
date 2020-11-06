#!/usr/bin/env bash

EXTS_DIR="`dirname "$0"`/../external"

if [[ ! (-e "$EXTS_DIR"/cgltf) ]]; then
  cmake -E chdir "$EXTS_DIR" git clone --recursive https://github.com/jkuhlmann/cgltf.git
fi
