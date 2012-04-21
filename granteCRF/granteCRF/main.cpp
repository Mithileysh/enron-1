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
        cerr << "Usage: ./a.out [input file] [split percentage] [order]" << endl;
        exit(1);
    }

    SVMLightReader reader(argv[1]);

    cout << "Number of unique topics: " << reader.card()[0] << endl;
    cout << "Number of unique words: " << reader.card()[1] << endl;

    //reader.print();

    unsigned int order = (unsigned int) atoi(argv[3]);
    FactorBuilder builder(order);

    unsigned int partial_size = (unsigned int) reader.data_vector().size() * atof(argv[2]);

    vector< unsigned int > var_set;
    vector< unsigned int > partial_obs;

    BOOST_ASSERT(partial_size < reader.data_vector().size());

    for (unsigned int di = 0; di < partial_size; di++) {
        var_set.push_back(di);
        partial_obs.push_back(reader.observations()[di]);
    }

    builder.build(reader.data_vector(),
            reader.data_idx_vector(),
            reader.card(),
            partial_obs,
            var_set);

    // Learning

    builder.estimateParameters(partial_obs, var_set);

    cout << "Topic Params Learned" << endl;
    builder.printTopicParams();

    cout << "Words Params Learned" << endl;
    builder.printWordsParams();

    // Test

    vector< unsigned int > test_obs;


    for (unsigned int ti = 0; ti < reader.data_vector().size() - partial_size; ti++) {
        unsigned int offset = partial_size;
        test_obs.push_back(reader.observations()[ti + offset]);
    }

    cout << "trainer accuracy: " << builder.trainingAccuracy(test_obs) << endl;


    return 0;
}




