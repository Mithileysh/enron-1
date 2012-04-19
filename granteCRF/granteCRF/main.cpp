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

#include "svmlightReader.h"
#include "factorBuilder.h"

using namespace std;
using namespace boost;
using namespace Grante;
using namespace pyongjoo;

int main (int argc, const char * argv[])
{

    SVMLightReader reader(argv[1]);

    //reader.print();

    FactorBuilder builder;

    builder.build(reader.data_vector(),
            reader.data_idx_vector(),
            reader.observations(),
            reader.card());

    builder.estimateParameters();

    cout << "trainer accuracy: " << builder.trainingAccuracy() << endl;





    return 0;
}




