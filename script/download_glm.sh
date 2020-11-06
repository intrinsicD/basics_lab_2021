#!/usr/bin/env bash

EXTS_DIR="`dirname "$0"`/../external"

if [[ ! (-e "$EXTS_DIR"/glm) ]]; then
  cmake -E chdir "$EXTS_DIR"/ git clone --recursive https://github.com/g-truc/glm.git
fi
