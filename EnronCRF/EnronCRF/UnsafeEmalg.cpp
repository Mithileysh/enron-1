//
//  UnsafeEmalg.cpp
//  EnronCRF
//
//  Created by Yongjoo Park on 4/14/12.
//  Copyright (c) 2012 University of Michigan. All rights reserved.
//

#include <iostream>
#include <dai/emalg.h>
#include "UnsafeEmalg.h"

using namespace dai;
using namespace std;

UnsafeEmalg::UnsafeEmalg(const Evidence &evidence, InfAlg &estep, std::istream &mstep_file)
: EMAlg(evidence, estep, mstep_file) {
    
}

Real UnsafeEmalg::iterateWithoutEstep() {
    Real likelihood;
    
    for(size_t i = 0; i < _msteps.size(); ++i)
        likelihood = iterateWithoutEstep(_msteps[i]);
    
    _lastLogZ.push_back(likelihood);
    ++_iters;
    
    return likelihood;
}

/** Only maximization step included
 *  
 *  This function performs maximization based on the evidence collected from tab file
 *  Since there's no belief propagation stage, it does not return proper likelihood.
 */
Real UnsafeEmalg::iterateWithoutEstep(MaximizationStep &mstep) {
    Real logZ = 0;
    Real likelihood = 0;

    // Expectation calculation
    int nProcessed = 0;
    
    for( Evidence::const_iterator e = _evidence.begin(); e != _evidence.end(); ++e ) { 
        InfAlg* clamped = _estep.clone();
        
        // Apply evidence
        for( Evidence::Observation::const_iterator i = e->begin(); i != e->end(); ++i )
            clamped->clamp( clamped->fg().findVar(i->first), i->second );
        
        mstep.addExpectations( *clamped );
        
        delete clamped;
        
        nProcessed++;
        
        if (nProcessed % 10000 == 0) {
            cout << nProcessed << " number of samples processed" << endl;
        }
    }
    
    // Maximization of parameters
    mstep.maximize(_estep.fg());
    
    return likelihood;
}

Real UnsafeEmalg::iterate(MaximizationStep &mstep) {
    Real logZ = 0;
    Real likelihood = 0;
    
    _estep.run();
    logZ = _estep.logZ();
    
    // Expectation calculation
    for( Evidence::const_iterator e = _evidence.begin(); e != _evidence.end(); ++e ) { 
        InfAlg* clamped = _estep.clone();
        
        // Apply evidence
        for( Evidence::Observation::const_iterator i = e->begin(); i != e->end(); ++i )
            clamped->clamp( clamped->fg().findVar(i->first), i->second );
        
        clamped->init();
        clamped->run();
        
        likelihood += clamped->logZ() - logZ;
        
        mstep.addExpectations( *clamped );
        
        delete clamped;
    }       
    
    // Maximization of parameters
    mstep.maximize( _estep.fg() );
    
    return likelihood;
}


Real UnsafeEmalg::iterate() {
    return EMAlg::iterate();
}