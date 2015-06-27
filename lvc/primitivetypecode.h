//
//  primitivetypeenum.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/24.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <cassert>

enum PrimitiveTypeCode {
    PrimitiveTypeCodeChar,
    PrimitiveTypeCodeShort,
    PrimitiveTypeCodeInt,
    PrimitiveTypeCodeLong,
    PrimitiveTypeCodeUchar,
    PrimitiveTypeCodeUshort,
    PrimitiveTypeCodeUint,
    PrimitiveTypeCodeUlong,
    PrimitiveTypeCodeFloat,
    PrimitiveTypeCodeDouble,
    PrimitiveTypeCodeBool,
};

inline const char* debugStringForPrimitiveType(PrimitiveTypeCode c) {
    static const char* vals[] = {
        "PrimitiveTypeCodeChar",
        "PrimitiveTypeCodeShort",
        "PrimitiveTypeCodeInt",
        "PrimitiveTypeCodeLong",
        "PrimitiveTypeCodeUchar",
        "PrimitiveTypeCodeUshort",
        "PrimitiveTypeCodeUint",
        "PrimitiveTypeCodeUlong",
        "PrimitiveTypeCodeFloat",
        "PrimitiveTypeCodeDouble",
        "PrimitiveTypeCodeBool",
    };

    int numVals = sizeof(vals) / sizeof(vals[0]);
    assert(c >= 0 && c < numVals);
    return vals[c];
}
