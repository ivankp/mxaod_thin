#!/bin/bash

dir=/eos/atlas/atlascerngroupdisk/phys-higgs/HSG1/MxAOD/h024/mc16a/Nominal
out=~/space/thin/h024/mc16a

mkdir -p $out

for f in \
mc16a.PowhegPy8_NNLOPS_ggH125.MxAODDetailed.e5607_s3126_r9364_p3665.h024.root \
mc16a.PowhegPy8EG_NNPDF30_VBFH125.MxAODDetailed.e6970_s3126_r9364_p3705.h024.root \
mc16a.PowhegPy8_WmH125J.MxAODDetailed.e5734_s3126_r9364_p3665.h024.root \
mc16a.PowhegPy8_WpH125J.MxAODDetailed.e5734_s3126_r9364_p3665.h024.root \
mc16a.PowhegPy8_ZH125J.MxAODDetailed.e5743_s3126_r9364_p3665.h024.root \
mc16a.PowhegPy8_ggZH125.MxAODDetailed.e5762_s3126_r9364_p3665.h024.root \
mc16a.PowhegPy8_ttH125.MxAODDetailed.e6503_s3126_r9364_p3665.h024.root \
mc16a.PowhegPy8_bbH125.MxAODDetailed.e6050_s3126_r9364_p3665.h024.root \
mc16a.aMCnloHwpp_tWH125_yt_plus1.MxAODDetailed.e4394_s3126_r9364_p3665.h024.root
do
  ./bin/mxaod_thin $@ \
    -i $dir/$f \
    -o $out/$(sed -r 's/mc16a\.([^.]+).+/\1/' <<< $f).root
done
