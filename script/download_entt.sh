#!/usr/bin/env bash

EXTS_DIR="`dirname "$0"`/../exts"

if [[ ! (-e "$EXTS_DIR"/entt) ]]; then
  cmake -E chdir "$EXTS_DIR" git clone --recursive https://github.com/skypjack/entt.git
fi