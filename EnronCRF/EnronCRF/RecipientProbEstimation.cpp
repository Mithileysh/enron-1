//
//  RecipientProbEstimation.cpp
//  EnronCRF
//
//  Created by Yongjoo Park on 4/13/12.
//  Copyright (c) 2012 University of Michigan. All rights reserved.
//

#include <iostream>
#include "RecipientProbEstimation.h"

using namespace std;
using namespace dai;


RecipientProbEstimation::RecipientProbEstimation(size_t target_dimension, const Prob &pseudocounts)
: CondProbEstimation(target_dimension, pseudocounts) {
}


ParameterEstimation* RecipientProbEstimation::clone() const {
    return new RecipientProbEstimation(_target_dim, _initial_stats );
}


Prob RecipientProbEstimation::estimate() {
    
    // normalize pseudocounts
    int size = (int) _stats.size();
    
    for (int i = 0; i < size; i++) {
        cout << _stats[i] << " ";
    }
    cout << endl;
    
    // whenever parent index increase, parent variable changes
    // target parent; num
    // 0    0;  2
    // 1    0;  1
    // 0    1;  2
    // 1    1;  5
    for (size_t parent = 0; parent < size; parent += _target_dim) {
        
        // calculate norm
        size_t top = parent + _target_dim;
        Real norm = 0.0;
        
        for( size_t i = parent; i < top; ++i )
            norm += _stats[i];
        
        if( norm != 0.0 )
            norm = 1.0 / norm;
        
        // normalize
        for( size_t i = parent; i < top; ++i )
            _stats.set( i, _stats[i] * norm );
    }
    
    // reset _stats to _initial_stats
    Prob result = _stats;
    _stats = _initial_stats;
    
    return result;
}


ParameterEstimation* RecipientProbEstimation::factory(const PropertySet &p) {
    size_t target_dimension = p.getStringAs<size_t>("target_dim");
    size_t total_dimension = p.getStringAs<size_t>("total_dim");
    
    Real pseudo_count = 1;
    
    if( p.hasKey("pseudo_count") )
        pseudo_count = p.getStringAs<Real>("pseudo_count");
    
    return new RecipientProbEstimation(target_dimension, Prob(total_dimension, pseudo_count));
}
