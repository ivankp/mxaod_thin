#!/bin/bash

dir=/eos/atlas/atlascerngroupdisk/phys-higgs/HSG1/MxAOD/h021

./bin/mxaod_thin $@ -o ~/space/thin/h021/data_h021_1516.root -i \
$dir/data15/data15_13TeV.periodAllYear_3213ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data16/data16_13TeV.DS1_2500ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data16/data16_13TeV.DS2_7501ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data16/data16_13TeV.DS3_8725ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data16/data16_13TeV.DS4_5804ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data16/data16_13TeV.DS5_2209ipb.physics_Main.MxAOD.p3402.h021.root \
$dir/data16/data16_13TeV.DS6_6145ipb.physics_Main.MxAOD.p3402.h021.root

