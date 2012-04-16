//
//  libUtils.cpp
//  EnronCRF
//
//  Created by Yongjoo Park on 4/15/12.
//  Copyright (c) 2012 University of Michigan. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <fstream>

#include <dai/alldai.h>

#include "RecipientProbEstimation.h"
#include "UnsafeEmalg.h"
#include "libUtils.h"

using namespace std;
using namespace dai;

void YPTools::printFactor(const Factor &f) {
    cout << endl;
    cout << f.vars().size() << endl;
    
    for( VarSet::const_iterator i = f.vars().begin(); i != f.vars().end(); i++ )
        cout << i->label() << " ";
    
    cout << endl;
    
    for( VarSet::const_iterator i = f.vars().begin(); i != f.vars().end(); i++ )
        cout << i->states() << " ";
    
    cout << endl;
    
    size_t nr_nonzeros = 0;
    
    for( size_t k = 0; k < f.nrStates(); k++ )
        if( f[k] != (Real)0 )
            nr_nonzeros++;
    
    cout << nr_nonzeros << endl;
    
    for( size_t k = 0; k < f.nrStates(); k++ )
        if( f[k] != (Real)0 )
            cout << k << " " << setw((int) cout.precision() + 4) << f[k] << endl;
}

/** Read the data from files and output the learned factor graph
 *
 */
const Factor &YPTools::getAFactorWithLearnedParameter(int argc, const char *argv[]) {
    // argv[1]: factor graph file (very large number of factors)
    // argv[2]: tab file (tab file should hold only one evidence)
    // argv[3]: em file (should specify all the shared parameters)
    
    if (argc < 4) {
        cerr << "USAGE: a.out [fg file] [tab file] [em file]" << endl;
        exit(1);
    }
    
    const char *fgFile = argv[1];
    const char *tabFile = argv[2];
    const char *emFile = argv[3];
    
    // This is a custom parameter estimator
    ParameterEstimation::registerMethod("RecipientProbEstimation", RecipientProbEstimation::factory);
    
    // Read the factorgraph from the file
    FactorGraph fg;
    fg.ReadFromFile(fgFile);
    
    // Set the inferencer (act as a dummy here to use EM class)
    PropertySet infprops;
    infprops.set("verbose", (size_t) 1);
    infprops.set("updates", string("SEQRND"));  // SEQRND
    infprops.set("maxiter", (size_t) 3);  // Maximum number of iterations
    infprops.set("tol", 1e-5);          // Tolerance for convergence
    infprops.set("logdomain", false);
    InfAlg* inf = newInfAlg("BP", fg, infprops);
    inf->init();
    
//    cout << inf->fg();
    
    // Read evidence from file
    Evidence evid;
    ifstream estream(tabFile);
    evid.addEvidenceTabFile(estream, fg);
    cerr << "Number of samples: " << evid.nrSamples() << endl;
    
    // Read EM specification
    // Here using custom EM algorithm class which extended base class
    // Extended class does not include inference step, and fast for the factor graphs
    // with fully observed evidence
    ifstream emstream(emFile);
    UnsafeEmalg em(evid, *inf, emstream);
    
    // Iterate EM until convergence
    // Iterate only once, since no inference is used internally
    for (int i = 0; i < 1; i++) {
        // This is a custom function without any inference step inside
        Real l = em.iterateWithoutEstep();
        cerr << "Iteration " << em.Iterations() << " likelihood: " << l <<endl;
    }
    
//    cout << inf->fg();
    
    return inf->fg().factor(0);
}