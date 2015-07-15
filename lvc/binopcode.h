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
    START_SAMETYPE_OPS,
    BinopCodeAdd,
    BinopCodeSubtract,
    BinopCodeMultiply,
    BinopCodeDivide,
    END_SAMETYPE_OPS,
    
    START_BOOL_OPS,
    BinopCodeEquals, // used for ==, not =
    BinopCodeNotEquals,
    BinopCodeLargerThan,
    BinopCodeSmallerThan,
    BinopCodeLargerThanEquals,
    BinopCodeSmallerThanEquals,
    END_BOOL_OPS,
};

inline bool doesBinopEvaluateToOperandType(BinopCode c) {
    return (c > START_SAMETYPE_OPS && c < END_SAMETYPE_OPS);
}

inline bool doesBinopEvaluateToBoolean(BinopCode c) {
    return (c > START_BOOL_OPS && c < END_BOOL_OPS);
}

inline const char* debugStringForBinop(BinopCode c) {
    static const char* vals[] = {
        "START_SAMETYPE_OPS",
        "BinopCodeAdd",
        "BinopCodeSubtract",
        "BinopCodeMultiply",
        "BinopCodeDivide",
        "END_SAMETYPE_OPS",
        
        "START_BOOL_OPS",
        "BinopCodeEquals",
        "BinopCodeNotEquals",
        "BinopCodeLargerThan",
        "BinopCodeSmallerThan",
        "BinopCodeLargerThanEquals",
        "BinopCodeSmallerThanEquals",
        "END_BOOL_OPS",
    };
    
    int numVals = sizeof(vals) / sizeof(vals[0]);
    assert(c >= 0 && c < numVals);
    return vals[c];
}
