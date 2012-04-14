#!/bin/bash
MalletHome=/home/yongjoo/workspace/mallet-2.0.7
InputDir=exclusiveDataset/jeff.dasovich/*
OutputFile=exclusiveEnron.mallet
CMD="$MalletHome/bin/mallet import-dir \
--input $InputDir \
--output $OutputFile"
#echo $CMD
$CMD
