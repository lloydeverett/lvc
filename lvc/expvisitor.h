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
        
        switch (binOpExp.op) {
            case LargerThan:
                
                break;
            case SmallerThan:
                
                break;
            case LargerThanEquals:
                
                break;
            case SmallerThanEquals:
                
                break;
            case EqualsEquals:
                
                break;
            case Plus:
                
                break;
            case Minus:
                
                break;
            case Multiply:
                
                break;
            case Divide:
                
                break;
            default:
                assert(false);
        }
#warning TODO: implement
    }
    virtual void visit(ast::IntegerLiteralExp &integerLiteralExp) {
        returnThis = llvm::ConstantInt::get(llvm::Type::getInt32Ty(c), integerLiteralExp.value.data(), 10);
    }
};
