#!/bin/bash

dir=/eos/atlas/atlascerngroupdisk/phys-higgs/HSG1/MxAOD/h021/mc16a/Nominal

for f in \
mc16a.PowhegPy8_NNLOPS_ggH125.MxAODDetailed.p3472.h021.root 
# mc16a.PowhegPy8_NNPDF30_VBFH125.MxAODDetailed.p3472.h021.root \
# mc16a.PowhegPy8_WmH125J.MxAODDetailed.p3472.h021.root \
# mc16a.PowhegPy8_WpH125J.MxAODDetailed.p3472.h021.root \
# mc16a.PowhegPy8_ZH125J.MxAODDetailed.p3472.h021.root \
# mc16a.PowhegPy8_ggZH125.MxAODDetailed.p3472.h021.root \
# mc16a.aMCnloPy8_ttH125.MxAODDetailed.p3472.h021.root \
# mc16a.aMCnloPy8_bbH125_yb2.MxAODDetailed.p3472.h021.root \
# mc16a.aMCnloPy8_bbH125_ybyt.MxAODDetailed.p3472.h021.root \
# mc16a.aMCnloHwpp_tWH125_yt_plus1.MxAODDetailed.p3472.h021.root
do
  ./bin/mxaod_thin \
    -i ${dir}/$f \
    -o ~/space/thin/h021/$(sed -r 's/mc16a\.([^.]+).+/\1/' <<< $f).root
done
