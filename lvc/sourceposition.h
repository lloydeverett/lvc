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
    colnumber col;
    rownumber row;
    
    SourcePosition(colnumber col, rownumber row) : col(col), row(row) {
        
    }
};
