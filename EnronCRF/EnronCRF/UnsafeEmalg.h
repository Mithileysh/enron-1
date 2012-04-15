//
//  UnsafeEmalg.h
//  EnronCRF
//
//  Created by Yongjoo Park on 4/14/12.
//  Copyright (c) 2012 University of Michigan. All rights reserved.
//

#ifndef EnronCRF_UnsafeEmalg_h
#define EnronCRF_UnsafeEmalg_h

#include <dai/emalg.h>

using namespace dai;

class UnsafeEmalg : public EMAlg {
    
public:
    UnsafeEmalg(const Evidence &evidence, InfAlg &estep, std::istream &mstep_file);
    
    virtual Real iterate(MaximizationStep &mstep);
    virtual Real iterate();
};


#endif
