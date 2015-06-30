//
//  stmtvisitor.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/30.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "ast.h"
#include "irgenconfig.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "inodevisitor.h"
#include "expvisitor.h"

class StmtVisitor : INodeVisitor {
private:
    llvm::IRBuilder<>& builder;
    ExpVisitor& expVisitor;
    
public:
    StmtVisitor(llvm::IRBuilder<>& builder, ExpVisitor& expVisitor) : builder(builder), expVisitor(expVisitor) {}
    
    virtual void visit(ast::FunctionCalExplStmt& functionCallStmt) {
        
    }
    
    virtual void visit(ast::ReturnStmt& returnStmt) {
        if (returnStmt.returnedExpression_ptr) {
            std::unique_ptr<ast::IExp>& uniquePtr = *(returnStmt.returnedExpression_ptr);
            uniquePtr->accept(expVisitor);
            builder.CreateRet(expVisitor.returnValue());
        }
        else {
            builder.CreateRetVoid();
        }
    }
    
    virtual void visit(ast::VariableDeclStmt& variableDeclStmt) {
        
    }
    
    virtual void visit(ast::IfStmt& ifStmt) {
        
    }
    
    virtual void visit(ast::BlockStmt& blockStmt) {
        
    }
};
