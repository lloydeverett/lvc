//
//  llvmirgenconfig.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/15.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "llvmincludes.h"

struct LLVMIRGenConfig {
    #warning TODO: use preserveNames
    int numBitsUsedByBooleans;
    bool preserveNames;
    llvm::Module* targetModule;
    llvm::LLVMContext& context() {
        return targetModule->getContext();
    }
};
