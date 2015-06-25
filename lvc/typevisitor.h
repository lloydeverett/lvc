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
             case ast::PrimitiveType::CHAR_CODE:
             case ast::PrimitiveType::UCHAR_CODE:
                returnThis = llvm::Type::getInt8Ty(c); return;
             case ast::PrimitiveType::SHORT_CODE:
             case ast::PrimitiveType::USHORT_CODE:
                returnThis = llvm::Type::getInt16Ty(c); return;
             case ast::PrimitiveType::INT_CODE:
             case ast::PrimitiveType::UINT_CODE:
                returnThis = llvm::Type::getInt32Ty(c); return;
             case ast::PrimitiveType::LONG_CODE:
             case ast::PrimitiveType::ULONG_CODE:
                returnThis = llvm::Type::getInt64Ty(c); return;
             case ast::PrimitiveType::FLOAT_CODE:
                returnThis = llvm::Type::getFloatTy(c); return;
             case ast::PrimitiveType::DOUBLE_CODE:
                returnThis = llvm::Type::getDoubleTy(c); return;
             case ast::PrimitiveType::BOOL_CODE:
                returnThis = llvm::Type::getIntNTy(c, config.bitsUsedByBooleanType);
        }
    }
};
