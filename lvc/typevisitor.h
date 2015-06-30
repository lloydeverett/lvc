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
    llvm::LLVMContext &context;
    IRGenConfig &config;
    llvm::Type* returnThis;
    
public:
    TypeVisitor(llvm::LLVMContext &context, IRGenConfig &config) : context(context), config(config), returnThis(nullptr) {}
    llvm::Type* returnValue() {
        assert(returnThis != nullptr);
        return returnThis;
    }
    virtual void visit(ast::VoidType &v) {
        returnThis = llvm::Type::getVoidTy(context);
    }
    virtual void visit(ast::IntegerType& integerType) {
        // LLVM doesn't care about signedness at this point.
        // Only when we do certain operations (e.g. divide)
        // will we have to supply the sign.
        returnThis = llvm::Type::getIntNTy(context, integerType.numBits);
    }
    virtual void visit(ast::BoolType& boolType) {
        returnThis = llvm::Type::getIntNTy(context, config.numBitsUsedByBooleans);
    }
    virtual void visit(ast::FloatingPointType& floatingPointType) {
        if (floatingPointType.variation == ast::FloatingPointType::VariationFloat)
            returnThis = llvm::Type::getFloatTy(context);
        else if (floatingPointType.variation == ast::FloatingPointType::VariationDouble)
            returnThis = llvm::Type::getDoubleTy(context);
        else
            assert(false);
    }
};
