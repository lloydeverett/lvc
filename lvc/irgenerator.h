//
//  irgenerator.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/30.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "llvm/IR/Verifier.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "typevisitor.h"
#include "expvisitor.h"
#include "stmtvisitor.h"

class IRGenerator {
private:
    // The order of these members is important.
    llvm::Module* targetModule;
    llvm::LLVMContext& context;
    llvm::IRBuilder<> builder;
    IRGenConfig config;
    TypeVisitor typeVisitor;
    ExpVisitor expVisitor;
    StmtVisitor stmtVisitor;
    
public:
    IRGenerator(llvm::Module* targetModule, IRGenConfig config) : targetModule(targetModule), context(targetModule->getContext()), builder(context), config(config),
                                                                  typeVisitor(context, config), expVisitor(builder), stmtVisitor(builder, expVisitor) {}
    
    
};
