#!/bin/bash

in1=out/h021/mc16a
in2=out/h021/mc16d
out=out/h021/mc16_ad

mkdir -p $out

mca=0.451721 # h021
mcd=0.548279 # h021

for f in $(ls $in1); do
  ./bin/merge_mc ${in1}/${f}:${mca} ${in2}/${f}:${mcd} -o ${out}/$f
done

