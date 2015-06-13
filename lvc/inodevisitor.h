//
//  inodevisitor.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/13.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "ast.h"

class INodeVisitor {
public:
    virtual void visit(ast::Function &function) = 0;
    virtual void visit(ast::ArgumentDecl &argumentDecl) = 0;
    virtual void visit(ast::FunctionDecl &functionDecl) = 0;
    virtual void visit(ast::VariableDecl &variableDecl) = 0;
    virtual void visit(ast::BinOpExp &binOpExp) = 0;
    virtual void visit(ast::FunctionCallExp &functionCallExp) = 0;
    virtual void visit(ast::IntegerLiteralExp &integerLiteralExp) = 0;
    virtual void visit(ast::VariableExp &variableExp) = 0;
    virtual void visit(ast::FunctionCallStmt &functionCallStmt) = 0;
    virtual void visit(ast::ReturnStmt &returnStmt) = 0;
    virtual void visit(ast::VariableDeclStmt &variableDeclStmt) = 0;
    virtual void visit(ast::PrimitiveType &p) = 0;
    virtual void visit(ast::Module &module) = 0;
};
