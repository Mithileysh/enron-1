#!/bin/bash
MalletHome=/home/yongjoo/workspace/mallet-2.0.7
CMD="$MalletHome/bin/mallet import-dir --input dataset/jeff.dasovich/* --output topic-input.mallet --keep-sequence --remove-stopwords"
#echo $CMD
$CMD
