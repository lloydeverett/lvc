//
//  binopcode.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/23.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <cassert>

enum BinopCode {
    BinopCodeAdd,
    BinopCodeSubtract,
    BinopCodeMultiply,
    BinopCodeDivide,
};

inline const char* debugStringForBinop(BinopCode c) {
    static const char* vals[] = {
        "BinopCodeAdd",
        "BinopCodeSubtract",
        "BinopCodeMultiply",
        "BinopCodeDivide",
    };
    
    int numVals = sizeof(vals) / sizeof(vals[0]);
    assert(c >= 0 && c < numVals);
    return vals[c];
}