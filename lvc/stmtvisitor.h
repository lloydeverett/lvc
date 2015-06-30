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

class StmtVisitor : public INodeVisitor {
private:
    llvm::IRBuilder<>& builder;
    ExpVisitor& expVisitor;
    
public:
    StmtVisitor(llvm::IRBuilder<>& builder, ExpVisitor& expVisitor) : builder(builder), expVisitor(expVisitor) {}
    
    virtual void visit(ast::FunctionCalExplStmt& functionCallExpStmt) {
        functionCallExpStmt.functionCallExp.accept(expVisitor);
    }
    
    virtual void visit(ast::ReturnStmt& returnStmt) {
        if (returnStmt.optReturnedExpression) {
            auto& returnedExpression = *returnStmt.optReturnedExpression;
            returnedExpression->accept(expVisitor);
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
        for (std::unique_ptr<ast::IStmt>& statement : blockStmt.statements) {
            statement->accept(*this);
        }
    }
};
