#!/bin/bash

dir=/eos/atlas/atlascerngroupdisk/phys-higgs/HSG1/MxAOD/h021/mc16d/Nominal

for f in \
mc16d.PowhegPy8_NNLOPS_ggH125.MxAODDetailed.p3472.h021.root \
mc16d.PowhegPy8_NNPDF30_VBFH125.MxAODDetailed.p3524.h021.root \
mc16d.PowhegPy8_WmH125J.MxAODDetailed.p3472.h021.root \
mc16d.PowhegPy8_WpH125J.MxAODDetailed.p3472.h021.root \
mc16d.PowhegPy8_ZH125J.MxAODDetailed.p3472.h021.root \
mc16d.PowhegPy8_ggZH125.MxAODDetailed.p3472.h021.root \
mc16d.PowhegPy8_ttH125.MxAODDetailed.p3524.h021.root \
mc16d.PowhegPy8_bbH125.MxAODDetailed.p3524.h021.root \
mc16d.aMCnloHwpp_tWH125_yt_plus1.MxAODDetailed.p3472.h021.root
do
  ./bin/mxaod_thin \
    -i ${dir}/$f \
    -o ~/space/thin/h021/mc16d/$(sed -r 's/mc16d\.([^.]+).+/\1/' <<< $f).root
done
