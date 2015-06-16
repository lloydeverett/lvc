//
//  typevisitor.h
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
#include "irgenconfig.h"

class TypeVisitor : public INodeVisitor {
private:
    llvm::LLVMContext &c;
    IRGenConfig &config;
    llvm::Type* returnThis;
    
public:
    TypeVisitor(llvm::LLVMContext &context, IRGenConfig &config) : c(context), config(config), returnThis(NULL) {
        
    }
    llvm::Type* returnValue() {
        return returnThis;
    }
    virtual void visit(ast::PrimitiveType &p) {
        PrimitiveTypeEnum name = p.name;
        if (name == Int || name == Uint) {
            returnThis = llvm::Type::getIntNTy(c, config.bitsUsedByIntsWithUnspecifiedBitWidth);
        }
        else if (name == Int8 || name == Uint8) {
            returnThis = llvm::Type::getInt8Ty(c);
        }
        else if (name == Int16 || name == Uint16) {
            returnThis = llvm::Type::getInt16Ty(c);
        }
        else if (name == Int32 || name == Uint32) {
            returnThis = llvm::Type::getInt32Ty(c);
        }
        else if (name == Int64 || name == Uint64) {
            returnThis = llvm::Type::getInt64Ty(c);
        }
        else if (name == Bool) {
            returnThis = llvm::Type::getIntNTy(c, config.bitsUsedByBooleanType);
        }
        else if (name == Float) {
            returnThis = llvm::Type::getFloatTy(c);
        }
        else if (name == Double) {
            returnThis = llvm::Type::getDoubleTy(c);
        }
        else {
            assert(false);
        }
    }
};
