//
//  llvmirgenerator.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/15.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <vector>
#include "llvmincludes.h"
#include "llvmirgenconfig.h"
#include "llvmirgentypevisitor.h"
#include <llvm/IR/GlobalValue.h>
#include "ast.h"

class LLVMIRGenerator {
private:
    LLVMIRGenConfig config;
    LLVMIRGenTypeVisitor typeVisitor;
    llvm::Module* targetModule() { return config.targetModule; }
    llvm::LLVMContext& context() { return config.context(); }
    llvm::IRBuilder<> builder;
    
public:
    typedef llvm::Value* VariableDeclAnnotations;
    typedef llvm::Value* FunctionDeclAnnotations;
    typedef llvm::Value* TemporaryAnnotations;
    typedef LLVMIRGenConfig GenConfig;
    
    LLVMIRGenerator(LLVMIRGenConfig config) : config(config), typeVisitor(&config), builder(context()) {}
    
    llvm::Value* genGlobalDecl(const ast::VariableDeclStmt& variableDeclStmt) {
        ast::IType& astType = *variableDeclStmt.decl.type.get();
        astType.accept(typeVisitor);
        llvm::Type* llvmType = typeVisitor.returnValue();
        llvm::GlobalVariable* global = new llvm::GlobalVariable(*targetModule(), llvmType, astType.isConstant(), llvm::GlobalValue::PrivateLinkage, );
        return global;
    }
    llvm::Value* genFunctionDecl(const ast::FunctionDecl& functionDecl) {
        
    }
};
