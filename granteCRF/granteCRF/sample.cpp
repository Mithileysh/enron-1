//
//  main.cpp
//  granteCRF
//
//  Created by Yongjoo Park on 4/16/12.
//  Copyright (c) 2012 University of Michigan. All rights reserved.
//

#include <iostream>
#include <vector>
#include <FactorType.h>
#include <FactorGraph.h>
#include <FactorGraphModel.h>
#include <ParameterEstimationMethod.h>
#include <InferenceMethod.h>
#include <TreeInference.h>
#include <MaximumLikelihood.h>

using namespace std;

int main (int argc, const char * argv[])
{
    // Read in training data
    vector<Grante::ParameterEstimationMethod::labeled_instance_type>
        training_data;
    vector<Grante::InferenceMethod*> inf_method;

    Grante::FactorGraphModel model;

    // Create one unary factor type: letter 'a'-'z'
    std::vector<unsigned int> card(1, 2);
    std::vector<double> w(4*2, 0.0);
    Grante::FactorType* ft = new Grante::FactorType("unary", card, w);
    model.AddFactorType(ft);

    // Create the pairwise factor
    std::vector<unsigned int> card_pw(2, 3);
    std::vector<double> w_pw(3*3, 0.0);
    Grante::FactorType* ft_pw = new Grante::FactorType("pairwise", card_pw, w_pw);
    //model.AddFactorType(ft_pw);

    // Add nodes
    std::vector<unsigned int> fg_varcard(1, 2);
    Grante::FactorGraph* fg = new Grante::FactorGraph(&model, fg_varcard);

    // Add factors
    std::vector<unsigned int> fac_pw_varindex(2);
    fac_pw_varindex[0] = 0;
    fac_pw_varindex[1] = 1;
    std::vector<double> data_pw_empty;
    Grante::Factor* fac_pw =
        new Grante::Factor(ft_pw, fac_pw_varindex, data_pw_empty);
    //fg->AddFactor(fac_pw);

    vector<unsigned int> fac_una_varindex(1, 0);
    vector<double> data;
    data.push_back(1.0);
    data.push_back(0);
    data.push_back(2.0);
    data.push_back(3.0);
    Grante::Factor *fac_una =
        new Grante::Factor(ft, fac_una_varindex, data);
    fg->AddFactor(fac_una);

    // ground truth label (set an observation)
    std::vector<unsigned int> fg_label;
    fg_label.push_back(1);

    // Add observation
    Grante::ParameterEstimationMethod::labeled_instance_type lit(fg,
            new Grante::FactorGraphObservation(fg_label));
    training_data.push_back(lit);

    // Add inference method: here its a chain, hence use the tree inference
    // method
    inf_method.push_back(new Grante::TreeInference(fg));

    // Train the model using regularized maximum likelihood estimation
    Grante::MaximumLikelihood mle(&model);
    mle.SetupTrainingData(training_data, inf_method);
    mle.Train(1.0e-4);
    std::cout << "Finished training." << std::endl;

    for (vector<double>::iterator i = ft->Weights().begin();
            i != ft->Weights().end(); ++i)
    {
        cout << *i  << endl;
    }

    // Perform MAP prediction
    Grante::TreeInference tinf(fg); // fg
    fg->ForwardMap();       // update energies
    vector<unsigned int> map_state;
    tinf.MinimizeEnergy(map_state);

    cout << "Map States:" << endl;

    for (vector<unsigned int>::iterator i = map_state.begin();
            i != map_state.end(); ++i)
    {
        cout << *i  << endl;
    }

    return 0;
}




