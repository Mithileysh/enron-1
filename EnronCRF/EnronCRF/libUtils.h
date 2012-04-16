//
//  libUtils.h
//  EnronCRF
//
//  Created by Yongjoo Park on 4/15/12.
//  Copyright (c) 2012 University of Michigan. All rights reserved.
//

#ifndef EnronCRF_libUtils_h
#define EnronCRF_libUtils_h

#include <dai/alldai.h>

using namespace dai;
using namespace std;

class YPTools {
    
public:
    
    static const Factor &getAFactorWithLearnedParameter(int argc, const char *argv[]);
    static void printFactor(const Factor &f);
};


#endif
