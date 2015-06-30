//
//  irgenerator.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/30.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "llvm/IR/Verifier.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "typevisitor.h"
#include "expvisitor.h"
#include "stmtvisitor.h"
#include <cassert>

class IRGenerator {
private:
    // The order of these members is important.
    llvm::Module* targetModule;
    llvm::LLVMContext& context;
    llvm::IRBuilder<> builder;
    IRGenConfig config;
    TypeVisitor typeVisitor;
    ExpVisitor expVisitor;
    StmtVisitor stmtVisitor;
    llvm::Function* currentTargetFunction;
    
public:
    IRGenerator(llvm::Module* targetModule, IRGenConfig config) : targetModule(targetModule), context(targetModule->getContext()), builder(context), config(config),
                                                                  typeVisitor(context, config), expVisitor(builder), stmtVisitor(builder, expVisitor),
                                                                  currentTargetFunction(nullptr) {}
    
    void genModule(ast::Module& module) {
        for (auto& function : module.functions) {
            genFunction(function);
        }
    }
    
    void genFunction(ast::Function& function) {
        function.decl.returnType->accept(typeVisitor);
        llvm::Type* returnType = typeVisitor.returnValue();
        
        std::vector<llvm::Type*> argTypes;
        for (const auto& arg : function.decl.arguments) {
            arg.variableDecl.type->accept(typeVisitor);
            argTypes.push_back(typeVisitor.returnValue());
        }
        
        llvm::FunctionType* type = llvm::FunctionType::get(returnType, argTypes, false);
        auto targetFunction = llvm::Function::Create(type, llvm::GlobalValue::ExternalLinkage, function.decl.identifier, targetModule);
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(context, "entry", targetFunction);
        builder.SetInsertPoint(entryBlock);
        function.block.accept(stmtVisitor);
    }
};
