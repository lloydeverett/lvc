//
//  irgenvisitor.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/12.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "inodevisitor.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

class IRGenNodeVisitor : public INodeVisitor {
private:
    llvm::IRBuilder<> builder;
    llvm::Module *llvmmod;
    bool keepNames;
    
public:
    // Note that the meaning of the llvm module does not necessarily correspond with that of
    // ast::Module. Multiple ast::Module's could be built into the same llvm module.
    // This is why we accept the llvm module in the constructor.
    
    IRGenNodeVisitor(llvm::Module *llvmmodp, bool keepNames) : llvmmod(llvmmodp), builder(llvmmodp->getContext()), keepNames(keepNames) {
        
    }
    
    virtual void visit(ast::Module &module) override {
        
    }
    
    virtual void visit(ast::Function &function) override {
        
        llvm::FunctionType ft = llvm::FunctionType::get(<#llvm::Type *Result#>, <#ArrayRef<llvm::Type *> Params#>, <#bool isVarArg#>);
    }
    
    virtual void visit(ast::ArgumentDecl &argumentDecl) override {
        
    }
    
    virtual void visit(ast::FunctionDecl &functionDecl) override {
        
    }
    
    virtual void visit(ast::VariableDecl &variableDecl) override {
        
    }
    
    virtual void visit(ast::BinOpExp &binOpExp) override {
        
    }
    
    virtual void visit(ast::FunctionCallExp &functionCallExp) override {
        
    }
    
    virtual void visit(ast::IntegerLiteralExp &integerLiteralExp) override {
        
    }
    
    virtual void visit(ast::VariableExp &variableExp) override {
        
    }
    
    virtual void visit(ast::FunctionCallStmt &functionCallStmt) override {
        
    }
    
    virtual void visit(ast::ReturnStmt &returnStmt) override {
        
    }
    
    virtual void visit(ast::VariableDeclStmt &variableDeclStmt) override {
        
    }
    
    virtual void visit(ast::PrimitiveType &p) override {
        
    }
    
};
