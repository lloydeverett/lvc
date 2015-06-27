//
//  irgenvisitor.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/15.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "ast.h"
#include "inodevisitor.h"
#include "irgenconfig.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "typevisitor.h"
#include "expvisitor.h"

class IRGenVisitor : public INodeVisitor {
private:
    llvm::LLVMContext &llvmContext;
    //////////
    //
    // Be careful: member initialization relies on the order of these
    // members. (i.e. reorder with care)
    //
    //////////
    llvm::Module *llvmModule;
    llvm::IRBuilder<> builder;
    IRGenConfig config;
    //////////
    TypeVisitor typeVisitor;
    ExpVisitor expVisitor;
    //////////

public:
    IRGenVisitor(llvm::Module *llvmModuleParam, IRGenConfig config);
    llvm::Module *getLLLVMModule() {
        return llvmModule;
    }
    virtual void visit(ast::Function &function);
    virtual void visit(ast::ReturnStmt &returnStmt);
    virtual void visit(ast::Module &module);

};
