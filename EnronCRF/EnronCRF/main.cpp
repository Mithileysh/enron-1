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
#include <dai/jtree.h>
#include <dai/bp.h>
#include <dai/decmap.h>

#include "splinkler-example.h"
#include "RecipientProbEstimation.h"
#include "UnsafeEmalg.h"

using namespace std;
using namespace dai;

void runLearning(const char *fgFile, const char *tabFile, const char *emFile);

int main (int argc, const char *argv[])
{
//    const char *fgFile = "/Users/yongjoo/workspace/enron/EnronCRF/naive.fg";
//    const char *tabFile = "/Users/yongjoo/workspace/enron/EnronCRF/naive.tab";
//    const char *emFile = "/Users/yongjoo/workspace/enron/EnronCRF/naive.em";
    
    const char *fgFile = "/Users/yongjoo/workspace/enron/EnronCRF/naivebayes.fg";
    const char *tabFile = "/Users/yongjoo/workspace/enron/EnronCRF/naivebayes.tab";
    const char *emFile = "/Users/yongjoo/workspace/enron/EnronCRF/naivebayes.em";

    
    runLearning(fgFile, tabFile, emFile);
    
    return 0;
}

void runLearning(const char *fgFile, const char *tabFile, const char *emFile) {
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
    FactorGraph fg;
    fg.ReadFromFile(fgFile);
    
    PropertySet infprops;
    infprops.set("verbose", (size_t) 1);
    infprops.set("updates", string("SEQRND"));  // SEQRND
    infprops.set("maxiter", (size_t) 3);  // Maximum number of iterations
    infprops.set("tol", 1e-5);          // Tolerance for convergence
    infprops.set("logdomain", false);
    InfAlg* inf = newInfAlg("BP", fg, infprops);
    inf->init();
    
    // Read sample from file
    Evidence e;
    ifstream estream(tabFile);
    e.addEvidenceTabFile(estream, fg);
    cout << "Number of samples: " << e.nrSamples() << endl;
    
    // Read EM specification
    ifstream emstream(emFile);
    UnsafeEmalg em(e, *inf, emstream);
    
    // Iterate EM until convergence
    for (int i = 0; i < 5; i++) {
//    while( !em.hasSatisfiedTermConditions() ) {
        Real l = em.iterate();
        cout << "Iteration " << em.Iterations() << " likelihood: " << l <<endl;
    }
    
    // Output true factor graph
    cout << endl << "True factor graph:" << endl << "##################" << endl;
    cout.precision(12);
    cout << fg;
    
    // Output learned factor graph
    cout << endl << "Learned factor graph:" << endl << "#####################" << endl;
    cout.precision(12);
    cout << inf->fg();
    
    // Clean up
    delete inf;
}



