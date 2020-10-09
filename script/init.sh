#!/usr/bin/env bash

for f in `dirname "$0"`/download_*.sh;
do
  bash "$f";
done