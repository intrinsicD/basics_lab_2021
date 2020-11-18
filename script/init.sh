#!/usr/bin/env bash

cmake -E mkdir "../bin"

for f in `dirname "$0"`/download_*.sh;
do
  bash "$f";
done