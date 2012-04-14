//
//  main.cpp
//  EnronCRF
//
//  Created by Yongjoo Park on 4/12/12.
//  Copyright (c) 2012 University of Michigan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <dai/alldai.h>

#include "splinkler-example.h"
#include "RecipientProbEstimation.h"

using namespace std;
using namespace dai;

int main (int argc, const char * argv[])
{
    // This example program illustrates how to learn the
    // parameters of a Bayesian network from a sample of
    // the sprinkler network discussed at
    // http://www.cs.ubc.ca/~murphyk/Bayes/bnintro.html
    //
    // The factor graph file (sprinkler.fg) has to be generated first
    // by running example_sprinkler, and the data sample file 
    // (sprinkler.tab) by running example_sprinkler_gibbs
    
    // Add my custom probability estimator
    ParameterEstimation::registerMethod("RecipientProbEstimation", RecipientProbEstimation::factory);
    
    // Read the factorgraph from the file
    FactorGraph SprinklerNetwork;
    SprinklerNetwork.ReadFromFile( "/Users/yongjoo/workspace/enron/EnronCRF/naivebayes.fg" );
    
    // Prepare junction-tree object for doing exact inference for E-step
    PropertySet infprops;
    infprops.set( "verbose", (size_t)1 );
    infprops.set( "updates", string("HUGIN") );
    InfAlg* inf = newInfAlg( "JTREE", SprinklerNetwork, infprops );
    inf->init();
    
    // Read sample from file
    Evidence e;
    ifstream estream( "/Users/yongjoo/workspace/enron/EnronCRF/naivebayes.tab" );
    e.addEvidenceTabFile( estream, SprinklerNetwork );
    cout << "Number of samples: " << e.nrSamples() << endl;
    
    // Read EM specification
    ifstream emstream( "/Users/yongjoo/workspace/enron/EnronCRF/naivebayes.em" );
    EMAlg em(e, *inf, emstream);
    
    // Iterate EM until convergence
    while( !em.hasSatisfiedTermConditions() ) {
        Real l = em.iterate();
        cout << "Iteration " << em.Iterations() << " likelihood: " << l <<endl;
    }
    
    // Output true factor graph
    cout << endl << "True factor graph:" << endl << "##################" << endl;
    cout.precision(12);
    cout << SprinklerNetwork;
    
    // Output learned factor graph
    cout << endl << "Learned factor graph:" << endl << "#####################" << endl;
    cout.precision(12);
    cout << inf->fg();
    
    // Clean up
    delete inf;
    
    return 0;
}

