#!/bin/bash
MalletHome=/Users/yongjoo/workspace/mallet-2.0.7
InputDir=exclusiveDataset/jeff.dasovich/*
OutputFile=topic-input.mallet
CMD="$MalletHome/bin/mallet import-dir \
  --input $InputDir \
  --output $OutputFile \
  --keep-sequence \
  --remove-stopwords"
#echo $CMD
$CMD
