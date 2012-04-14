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
    size_t size = _stats.size();
    
    size_t total_sum = 0;
    
    // count the total number of occurrence   
    for (size_t i = 0; i < size; i++) {
        total_sum += _stats[i];
    }
    
    // whenever parent index increase, parent variable changes
    // target parent; num
    // 0    0;  2
    // 1    0;  1
    // 0    1;  2
    // 1    1;  5
    
    // next, count the number of occurrence at diagonal
    // this is equal to the numbers that parents and children are same.
    size_t column = 0;
    size_t diagonal_sum = 0;
    
    for (size_t parent = 0; parent < size; parent += _target_dim) {        
        diagonal_sum += _stats[parent+column];
        
        column++;
    }
    
    // based on the calculated 'sum' and 'diagonal sum', reset all the stat values
    size_t parent_dim = size / _target_dim;
    Real lamda = (Real) diagonal_sum / (Real) total_sum;
    Real others = (1 - lamda) / (parent_dim - 1);
    
    for (size_t i = 0; i < size; i++) {
        _stats.set(i, others);
    }
    
    column = 0;
    for (size_t parent = 0; parent < size; parent += _target_dim) {
        _stats.set(parent + column, lamda);
        column++;
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
