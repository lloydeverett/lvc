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
        switch (p.code) {
            case PrimitiveTypeCodeChar: returnThis = llvm::Type::getInt8Ty(c); return;
            case PrimitiveTypeCodeShort: returnThis = llvm::Type::getInt16Ty(c); return;
            case PrimitiveTypeCodeInt: returnThis = llvm::Type::getInt32Ty(c); return;
            case PrimitiveTypeCodeLong: returnThis = llvm::Type::getInt64Ty(c); return;
            case PrimitiveTypeCodeUchar: returnThis = llvm::Type::getInt8Ty(c); return;
            case PrimitiveTypeCodeUshort: returnThis = llvm::Type::getInt16Ty(c); return;
            case PrimitiveTypeCodeUint: returnThis = llvm::Type::getInt32Ty(c); return;
            case PrimitiveTypeCodeUlong: returnThis = llvm::Type::getInt64Ty(c); return;
            case PrimitiveTypeCodeFloat: returnThis = llvm::Type::getFloatTy(c); return;
            case PrimitiveTypeCodeDouble: returnThis = llvm::Type::getDoubleTy(c); return;
            case PrimitiveTypeCodeBool: returnThis = llvm::Type::getIntNTy(c, config.bitsUsedByBooleanType); return;
        }
        assert(false);
    }
};
