#!/usr/bin/env bash

EXTS_DIR="`dirname "$0"`/../external"

if [[ ! (-e "$EXTS_DIR"/eigen) ]]; then
  cmake -E chdir "$EXTS_DIR" git clone --recursive https://gitlab.com/libeigen/eigen.git
fi