//
//  irgenconfig.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/15.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once

struct IRGenConfig {
    int numBitsUsedByBooleans;
    bool preserveNamesInIR;
    #warning TODO: use preserveNames
};
