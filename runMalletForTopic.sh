#!/bin/bash
MalletHome=/home/yongjoo/workspace/mallet-2.0.7
InputFile=enronInTopics.mallet
CMD="$MalletHome/bin/mallet train-classifier \
--input $InputFile \
--cross-validation 10"
$CMD
