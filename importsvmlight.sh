#!/bin/bash
MalletHome=/home/yongjoo/workspace/mallet-2.0.7
InputFile=doc-topic-svmlight.txt
OutputFile=enronInTopics.mallet
CMD="$MalletHome/bin/mallet import-svmlight \
    --input $InputFile \
    --output $OutputFile"
$CMD
