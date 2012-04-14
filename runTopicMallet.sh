#!/bin/bash
MalletHome=/home/yongjoo/workspace/mallet-2.0.7
InputFile=topic-input.mallet
OutputFile=doc-topics.txt
CMD="$MalletHome/bin/mallet train-topics \
  --input $InputFile \
  --num-topics 1000 \
  --output-doc-topics $OutputFile"
$CMD
