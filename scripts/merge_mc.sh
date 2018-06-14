#!/bin/bash

in1=out/h021/mc16a
in2=out/h021/mc16d
out=out/h021/mc16_ad

for f in $(ls $in1); do
  ./bin/merge_mc ${in1}/${f}:0.451721 ${in2}/${f}:0.548279 -o ${out}/$f
done

