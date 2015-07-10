//
//  sourceposition.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/16.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <cstdint>

typedef uint64_t rownumber;
typedef uint64_t colnumber;
typedef uint64_t charcount;

class SourcePosition {
public:
    const rownumber row;
    const colnumber col;
    
    SourcePosition(rownumber row, colnumber col) : row(row), col(col) {}
};
