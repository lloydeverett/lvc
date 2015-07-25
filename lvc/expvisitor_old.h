//
//  expvisitor.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/16.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "irgendecls.h"

#include <vector>

class ExpVisitor : public INodeVisitor {
private:
    llvm::IRBuilder<>& builder;
    llvm::LLVMContext& context;
    llvm::Value* returnThis;
    LLVMSemanticAnalyzer& semanticAnalyzer;
public:
    ExpVisitor(llvm::IRBuilder<>& builder, LLVMSemanticAnalyzer& semanticAnalyzer) :
    builder(builder), context(builder.getContext()), semanticAnalyzer(semanticAnalyzer), returnThis(nullptr) {}
    llvm::Value* returnValue() {
        assert(returnThis != nullptr);
        return returnThis;
    }
    virtual void visit(ast::BinopExp& binOpExp) {
        
    }
    virtual void visit(ast::CallExp& callExp) {
        for (std::unique_ptr<ast::IExp>& argExp : callExp.passedArguments) {
            #warning TODO: implement
            assert(false);
        }
        std::vector<SemanticAnalyzer<llvm::Value*>::ExpAnalysis*> expAnalyses;
        auto analysis = semanticAnalyzer.analyzeCallExp(callExp, expAnalyses);
        builder.CreateCall(<#llvm::Value *Callee#>, <#ArrayRef<llvm::Value *> Args#>)
    }
    virtual void visit(ast::NumberLiteralExp& numberLiteralExp) {
#warning TODO: floats.
        returnThis = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), numberLiteralExp.value, 10);
    }
    virtual void visit(ast::VariableExp& variableExp) {
        
    }

};
