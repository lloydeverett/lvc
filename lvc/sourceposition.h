//
//  sourceposition.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/16.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "integertypedefs.h"

class SourcePosition {
public:
    rownumber row;
    colnumber col;
    
    SourcePosition(rownumber row, colnumber col) : row(row), col(col) {}
};
