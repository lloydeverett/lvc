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
    virtual void visit(ast::BinopExp &binopExp) {
        binopExp.lhs->accept(*this);
        llvm::Value* lhs = returnValue();
        
        binopExp.rhs->accept(*this);
        llvm::Value* rhs = returnValue();
        
        BinopCode code;
        switch (binopExp.code) {
            case BinopCodeAdd:
                returnThis = builder.CreateAdd(lhs, rhs); return;
            case BinopCodeSubtract:
                returnThis = builder.CreateSub(lhs, rhs); return;
            case BinopCodeMultiply:
                returnThis = builder.CreateMul(lhs, rhs); return;
            case BinopCodeDivide:
#warning TODO: THIS.
                assert(false);
        }
        assert(false);
    }
    virtual void visit(ast::IntegerLiteralExp &integerLiteralExp) {
#warning TODO: THIS.
        returnThis = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), integerLiteralExp.value.data(), 10);
    }
};
