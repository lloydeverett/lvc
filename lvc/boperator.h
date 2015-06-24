//
//  BOperator.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/23.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <cassert>

enum BOperator {
    BOperatorAdd,
    BOperatorSubtract,
    BOperatorMultiply,
    BOperatorDivide,
};

inline const char* debugStringForBOperator(BOperator o) {
    static const char* vals[] = {
        "BOperatorAdd",
        "BOperatorSubtract",
        "BOperatorMultiply",
        "BOperatorDivide",
    };
    
    int numVals = sizeof(vals) / sizeof(vals[0]);
    assert(o >= 0 && o < numVals);
    return vals[o];
}
