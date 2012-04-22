#!/bin/bash
MalletHome=/Users/yongjoo/workspace/mallet-2.0.7
InputFile=midtraining.mallet
CMD="$MalletHome/bin/mallet train-classifier \
--input $InputFile \
--cross-validation 10"
$CMD
