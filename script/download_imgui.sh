#!/usr/bin/env bash

EXTS_DIR="`dirname "$0"`/../libs/bcg_opengl/exts"

if [[ ! (-e "$EXTS_DIR"/imgui) ]];
then
  cmake -E chdir "$EXTS_DIR" git clone --recursive https://github.com/ocornut/imgui.git

  if [[ (-e "$EXTS_DIR"/imgui/imconfig.h) ]];
  then
      echo "#define IMGUI_IMPL_OPENGL_LOADER_GLAD" >> "$EXTS_DIR"/imgui/imconfig.h
  fi
fi