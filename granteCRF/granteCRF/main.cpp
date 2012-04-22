//
//  main.cpp
//  granteCRF
//
//  Created by Yongjoo Park on 4/16/12.
//  Copyright (c) 2012 University of Michigan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

#include <FactorType.h>
#include <FactorGraph.h>
#include <FactorGraphModel.h>
#include <ParameterEstimationMethod.h>
#include <InferenceMethod.h>
#include <TreeInference.h>
#include <MaximumLikelihood.h>

#include <boost/assert.hpp>

#include "svmlightReader.h"
#include "factorBuilder.h"

using namespace std;
using namespace boost;
using namespace Grante;
using namespace pyongjoo;

int main (int argc, const char * argv[])
{
    if (argc < 4) {
        cerr << "Usage: ./a.out [test file] [train file] \
            [observed index file]" << endl;
        exit(1);
    }

    const char *test_file = argv[1];
    const char *train_file = argv[2];
    const char *observed_index_file = argv[3];

    SVMLightReader reader(test_file, train_file, observed_index_file);

    cout << "Number of unique topics: " << reader.card()[0] << endl;
    cout << "Number of unique words: " << reader.card()[1] << endl;

    //reader.print();

    unsigned int order = 1;
    FactorBuilder builder(order);



    builder.build(reader.data_vector(),
            reader.data_idx_vector(),
            reader.card(),
            reader.partial_observations(),
            reader.var_subset());

    // Learning

    builder.estimateParameters(reader.partial_observations(),
            reader.var_subset());

    cout << "Topic Params Learned" << endl;
    builder.printTopicParams();

    cout << "Words Params Learned" << endl;
    //builder.printWordsParams();

    // Test

    vector< unsigned int > test_obs;


    cout << "trainer accuracy: "
        << builder.trainingAccuracy(reader.observations(), reader.var_hiddenset())
        << endl;


    return 0;
}




