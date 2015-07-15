//
//  binopcode.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/23.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <boost/optional.hpp>
#include <cassert>
#include "token.h"

enum BinopCode {
    START_SAMETYPE_OPS,
    BinopCodeAdd,
    BinopCodeSubtract,
    BinopCodeMultiply,
    BinopCodeDivide,
    BinopCodeModulo,
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

inline boost::optional<BinopCode> tryGetBinopCodeFromToken(const Token& token) {
    switch (token.getKind()) {
        case Plus: return BinopCodeAdd;
        case Minus: return BinopCodeSubtract;
        case Asterisk: return BinopCodeMultiply;
        case Slash: return BinopCodeDivide;
        case Percent: return BinopCodeModulo;
        case EqualsEquals: return BinopCodeEquals;
        case LargerThan: return BinopCodeLargerThan;
        case SmallerThan: return BinopCodeSmallerThan;
        case LargenThanEquals: return BinopCodeLargerThanEquals;
        case SmalerThanEquals: return BinopCodeSmallerThanEquals;
        case ExclamationEquals: return BinopCodeNotEquals;
        default:
            return boost::none;
    }
}

inline int getBinopCodePrecedence(BinopCode c) {
    switch (c) {
        case BinopCodeMultiply: case BinopCodeDivide: case BinopCodeModulo:
            return 10;
        case BinopCodeAdd: case BinopCodeSubtract:
            return 9;
        case BinopCodeSmallerThan: case BinopCodeLargerThan:
        case BinopCodeSmallerThanEquals: case BinopCodeLargerThanEquals:
            return 8;
        case BinopCodeEquals: case BinopCodeNotEquals:
            return 7;
        default:
            assert(false);
    }
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

inline bool doesBinopEvaluateToOperandType(BinopCode c) {
    return (c > START_SAMETYPE_OPS && c < END_SAMETYPE_OPS);
}

inline bool doesBinopEvaluateToBoolean(BinopCode c) {
    return (c > START_BOOL_OPS && c < END_BOOL_OPS);
}
