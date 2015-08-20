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
    llvm::Type* returnValue() {
        assert(returnThis != nullptr);
        return returnThis;
    }
public:
    LLVMIRGenTypeVisitor(LLVMIRGenConfig* const config) : config(config), returnThis(nullptr) {}
    virtual void visit(ast::VoidType& v) {
        returnThis = llvm::Type::getVoidTy(config->context());
    }
    virtual void visit(ast::IntegerType& integerType) {
        returnThis = getLLVMIntegerType(integerType);
    }
    virtual void visit(ast::BoolType& boolType) {
        returnThis = llvm::Type::getIntNTy(config->context(), 1);
    }
    virtual void visit(ast::FloatingPointType& floatingPointType) {
        if (floatingPointType.variation == ast::FloatingPointType::VariationFloat)
            returnThis = llvm::Type::getFloatTy(config->context());
        else if (floatingPointType.variation == ast::FloatingPointType::VariationDouble)
            returnThis = llvm::Type::getDoubleTy(config->context());
        else
            assert(false);
    }
    
    llvm::IntegerType* getLLVMIntegerType(ast::IntegerType& integerType) {
        return llvm::Type::getIntNTy(config->context(), integerType.numBits);
    }
    llvm::Type* getLLVMType(ast::IType& t) {
        t.accept(*this);
        return returnValue();
    }
};
