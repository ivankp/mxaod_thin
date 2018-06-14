#!/bin/bash

dir=/eos/atlas/atlascerngroupdisk/phys-higgs/HSG1/MxAOD/h021

./bin/mxaod_thin $@ -o ~/space/thin/h021/data_h021.root -i \
$dir/data15/data15_13TeV.periodAllYear_3213ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data16/data16_13TeV.DS1_2500ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data16/data16_13TeV.DS2_7501ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data16/data16_13TeV.DS3_8725ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data16/data16_13TeV.DS4_5804ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data16/data16_13TeV.DS5_2209ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data16/data16_13TeV.DS6_6145ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data17/data17_13TeV.DS1_7773ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data17/data17_13TeV.DS2_9607ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data17/data17_13TeV.DS3_11277ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data17/data17_13TeV.DS4_15156ipb.physics_Main.MxAOD.p3402.h021.root

