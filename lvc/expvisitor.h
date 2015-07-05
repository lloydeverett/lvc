//
//  expvisitor.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/16.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "inodevisitor.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <cassert>

class ExpVisitor : public INodeVisitor {
private:
    llvm::IRBuilder<>& builder;
    llvm::LLVMContext& context;
    llvm::Value* returnThis;
public:
    ExpVisitor(llvm::IRBuilder<>& builder) : builder(builder), context(builder.getContext()), returnThis(nullptr) {}
    llvm::Value* returnValue() {
        assert(returnThis != nullptr);
        return returnThis;
    }
    virtual void visit(ast::BinopExp& binOpExp) {
        
    }
    virtual void visit(ast::FunctionCallExp& functionCallExp) {
        
    }
    virtual void visit(ast::NumberLiteralExp& numberLiteralExp) {
#warning TODO: floats.
        returnThis = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), numberLiteralExp.value, 10);
    }
    virtual void visit(ast::VariableExp& variableExp) {
        
    }

};
