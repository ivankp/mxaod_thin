#!/bin/bash

dir=/eos/atlas/atlascerngroupdisk/phys-higgs/HSG1/MxAOD/h024
out=~/space/thin/h024

mkdir -p $out

./bin/mxaod_thin $@ -o $out/data_h024.root -i \
$dir/data15/data15_13TeV.periodAllYear_3219ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data16/data16_13TeV.DS1_4046ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data16/data16_13TeV.DS2_4290ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data16/data16_13TeV.DS3_4276ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data16/data16_13TeV.DS4_4236ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data16/data16_13TeV.DS5_4169ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data16/data16_13TeV.DS6_4353ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data16/data16_13TeV.DS7_4114ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data16/data16_13TeV.DS8_3481ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data17/data17_13TeV.DS1_7649ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data17/data17_13TeV.DS2_7484ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data17/data17_13TeV.DS3_7663ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data17/data17_13TeV.DS4_7443ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data17/data17_13TeV.DS5_7589ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data17/data17_13TeV.DS6_6479ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data18/data18_13TeV.DS1_8428ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data18/data18_13TeV.DS2_8340ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data18/data18_13TeV.DS3_8219ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data18/data18_13TeV.DS4_8043ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data18/data18_13TeV.DS5_8247ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data18/data18_13TeV.DS6_8471ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data18/data18_13TeV.DS7_8230ipb.physics_Main.MxAOD.p3704.h024.root \
$dir/data18/data18_13TeV.DS8_1960ipb.physics_Main.MxAOD.p3704.h024.root

