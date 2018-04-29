#!/bin/bash

dir=/eos/atlas/atlascerngroupdisk/phys-higgs/HSG1/MxAOD/h019

./bin/mxaod_thin -o ~/space/thin/data_h019_1516.root -i \
$dir/data15/data15_13TeV.periodAllYear_3213ipb.physics_Main.MxAOD.p3402.h019.root \
$dir/data16/data16_13TeV.DS1_2500ipb.physics_Main.MxAOD.p3402.h019.root \
$dir/data16/data16_13TeV.DS2_7501ipb.physics_Main.MxAOD.p3402.h019.root \
$dir/data16/data16_13TeV.DS3_8725ipb.physics_Main.MxAOD.p3402.h019.root \
$dir/data16/data16_13TeV.DS4_5804ipb.physics_Main.MxAOD.p3402.h019.root \
$dir/data16/data16_13TeV.DS5_2209ipb.physics_Main.MxAOD.p3402.h019.root \
$dir/data16/data16_13TeV.DS6_6145ipb.physics_Main.MxAOD.p3402.h019.root

