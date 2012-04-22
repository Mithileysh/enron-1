

#pragma once

#include "svmlightReader.h"

#include <FactorGraphModel.h>
#include <ParameterEstimationMethod.h>


using namespace std;
using namespace pyongjoo;
using namespace Grante;


namespace pyongjoo {

class FactorBuilder {

public:

    FactorBuilder(unsigned int order = 0);

    void build (vector< data_instance > &data_vector,
            vector< data_idx_instance > &data_idx_vector,
            vector< unsigned int > &card,
            vector< unsigned int > &partial_observations,
            vector< unsigned int > &var_subset);

    void estimateParameters (vector< unsigned int > &partial_observations,
            vector< unsigned int > &var_subset);

    double trainingAccuracy(vector< unsigned int > &real_label,
        vector<unsigned int> &var_subset);

    void printTopicParams();

    void printWordsParams();

private:

    FactorGraphModel _model;

    vector<Grante::ParameterEstimationMethod::partially_labeled_instance_type>
        _training_data;

    Grante::FactorGraph* _fg;

    // Order of the CRF
    // 0: Naive Bayes
    // 1: First Order CRF
    // 2: Second Order CRF
    // 3-: Multiple Order CRF
    unsigned int _chain_order;

};

}
