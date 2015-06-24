//
//  irgenconfig.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/15.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once

struct IRGenConfig {
    int bitsUsedByIntsWithUnspecifiedBitWidth; // number of bits used by the 'int' and 'uint' type in V
    int bitsUsedByBooleanType;
    bool preserveNames;
    #warning TODO: use preserveNames
};
