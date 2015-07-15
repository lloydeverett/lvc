//
//  llvmirgentypevisitor.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/16.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "llvmincludes.h"
#include "inodevisitor.h"
#include "llvmirgenconfig.h"

class LLVMIRGenTypeVisitor : public INodeVisitor {
private:
    LLVMIRGenConfig* const config;
    llvm::Type* returnThis;
    
public:
    LLVMIRGenTypeVisitor(LLVMIRGenConfig* const config) : config(config), returnThis(nullptr) {}
    llvm::Type* returnValue() {
        assert(returnThis != nullptr);
        return returnThis;
    }
    virtual void visit(ast::VoidType &v) {
        returnThis = llvm::Type::getVoidTy(config->context());
    }
    virtual void visit(ast::IntegerType& integerType) {
        // We ignore signedness for now even though it forms part of the type information:
        // the LLVM type system only needs this information when we do
        // certain operations e.g. divide.
        returnThis = llvm::Type::getIntNTy(config->context(), integerType.numBits);
    }
    virtual void visit(ast::BoolType& boolType) {
        returnThis = llvm::Type::getIntNTy(config->context(), config->numBitsUsedByBooleans);
    }
    virtual void visit(ast::FloatingPointType& floatingPointType) {
        if (floatingPointType.variation == ast::FloatingPointType::VariationFloat)
            returnThis = llvm::Type::getFloatTy(config->context());
        else if (floatingPointType.variation == ast::FloatingPointType::VariationDouble)
            returnThis = llvm::Type::getDoubleTy(config->context());
        else
            assert(false);
    }
};
