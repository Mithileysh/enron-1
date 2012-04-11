#!/bin/bash
MalletHome=/home/yongjoo/workspace/mallet-2.0.7
CMD="$MalletHome/bin/mallet train-topics --input topic-input.mallet --num-topics 100 --output-state topic-state.gz"
$CMD
