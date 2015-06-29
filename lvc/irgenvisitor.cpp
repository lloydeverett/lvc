//
//  irgenvisitor.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/15.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "irgenvisitor.h"
#include <vector>

IRGenVisitor::IRGenVisitor(llvm::Module *llvmModuleParam, IRGenConfig config)
: llvmContext(llvmModuleParam->getContext()), llvmModule(llvmModuleParam), config(config), builder(llvmContext),
  typeVisitor(llvmContext, this->config), expVisitor(llvmContext) {
    
}

void IRGenVisitor::visit(ast::Module &module) {
    for (ast::Function& function : module.functions) {
        function.accept(*this);
    }
}

void IRGenVisitor::visit(ast::Function &function) {
    // Determine return type in terms of LLVM constructs
    function.decl.returnType_ptr->accept(typeVisitor);
    llvm::Type* returnType = typeVisitor.returnValue();
    
    // Determine param types in terms of LLVM constructs
    std::vector<llvm::Type*> argTypes;
    argTypes.reserve(function.decl.arguments.size());
    for (const ast::ArgumentDecl& argdecl : function.decl.arguments) {
        argdecl.variableDecl.type_ptr->accept(typeVisitor);
        argTypes.push_back(typeVisitor.returnValue());
    }
    auto argTypesArrayRef = llvm::ArrayRef<llvm::Type*>(argTypes.data(), argTypes.size());
    
    // Create the function
    llvm::FunctionType* ft = llvm::FunctionType::get(returnType, argTypesArrayRef, false);
    #warning TODO: change linkages depending on function attrbs
    llvm::Function* f = llvm::Function::Create(ft, llvm::GlobalValue::LinkageTypes::ExternalLinkage,
                                               function.decl.identifier, llvmModule);
    
    // Set up the entry block
    llvm::BasicBlock *entryBlock = llvm::BasicBlock::Create(llvmContext, "entry", f);
    builder.SetInsertPoint(entryBlock);
    
    // We make this visitor parse each statement
    for (const std::unique_ptr<ast::IStmt>& statement_ptr : function.statements) {
        statement_ptr->accept(*this);
    }
}

void IRGenVisitor::visit(ast::ReturnStmt &returnStmt) {
    if (returnStmt.returnedExpression_ptr) {
        std::unique_ptr<ast::IExp> &exp_ptr = *returnStmt.returnedExpression_ptr;
        exp_ptr->accept(expVisitor);
        builder.CreateRet(expVisitor.returnValue());
    }
    else {
        builder.CreateRetVoid();
    }
}

void IRGenVisitor::visit(ast::BlockStmt &blockStmt) {
    
}
