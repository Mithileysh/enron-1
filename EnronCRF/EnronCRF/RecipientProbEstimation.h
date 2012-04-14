//
//  RecipientProbEstimation.h
//  EnronCRF
//
//  Created by Yongjoo Park on 4/13/12.
//  Copyright (c) 2012 University of Michigan. All rights reserved.
//

#ifndef EnronCRF_RecipientProbEstimation_h
#define EnronCRF_RecipientProbEstimation_h

#include <dai/emalg.h>

using namespace dai;

class RecipientProbEstimation : protected CondProbEstimation {
public:
    RecipientProbEstimation(size_t target_dimension, const Prob &pseudocounts);
    
    virtual ParameterEstimation* clone() const;
    
    virtual Prob estimate();
    static ParameterEstimation* factory(const PropertySet &p);
};

#endif
