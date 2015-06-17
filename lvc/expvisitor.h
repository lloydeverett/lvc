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
    llvm::LLVMContext &c;
    llvm::Value* returnThis;
public:
    ExpVisitor(llvm::LLVMContext &context) : c(context) {
        
    }
    llvm::Value* returnValue() {
        return returnThis;
    }
    virtual void visit(ast::BinOpExp &binOpExp) {
        binOpExp.accept(*this);
        llvm::Value* lhs = returnValue();
        binOpExp.accept(*this);
        llvm::Value* rhs = returnValue();
#warning TODO: implement
        switch (binOpExp.op) {
            case '+':
                break;
            case '-':
                break;
            case '*':
                break;
            case '/':
                break;
            default:
                assert(false);
                break;
        }
    }
    virtual void visit(ast::IntegerLiteralExp &integerLiteralExp) {
        returnThis = llvm::ConstantInt::get(llvm::Type::getInt32Ty(c), integerLiteralExp.value.data(), 10);
    }
};
