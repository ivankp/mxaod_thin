#!/bin/bash

dir=/eos/atlas/atlascerngroupdisk/phys-higgs/HSG1/MxAOD/h024/mc16e/Nominal
out=~/space/thin/h024/mc16e

mkdir -p $out

for f in \
mc16e.PowhegPy8_NNLOPS_ggH125.MxAODDetailed.e5607_s3126_r10724_p3665.h024.root \
mc16e.PowhegPy8EG_NNPDF30_VBFH125.MxAODDetailed.e6970_e5984_s3126_r10724_r10726_p3714.h024.root \
mc16e.PowhegPy8_WmH125J.MxAODDetailed.e5734_s3126_r10724_p3665.h024.root \
mc16e.PowhegPy8_WpH125J.MxAODDetailed.e5734_s3126_r10724_p3665.h024.root \
mc16e.PowhegPy8_ZH125J.MxAODDetailed.e5743_s3126_r10724_p3665.h024.root \
mc16e.PowhegPy8_ggZH125.MxAODDetailed.e5762_s3126_r10724_p3665.h024.root \
mc16e.PowhegPy8_ttH125.MxAODDetailed.e6503_s3126_r10724_p3665.h024.root \
mc16e.PowhegPy8_bbH125.MxAODDetailed.e6050_s3126_r10724_p3665.h024.root \
mc16e.aMCnloHwpp_tWH125_yt_plus1.MxAODDetailed.e4394_s3126_r10724_p3665.h024.root
do
  ./bin/mxaod_thin $@ \
    -i $dir/$f \
    -o $out/$(sed -r 's/mc16e\.([^.]+).+/\1/' <<< $f).root
done
