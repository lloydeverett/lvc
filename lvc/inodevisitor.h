//
//  inodevisitor.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/13.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "ast.h"
#include <cassert>

class INodeVisitor {
protected:
    INodeVisitor() {}
    
public:
    // If an assert in this file fails, the visitor
    // does not implement a visit function for the ast class you're trying to visit.
    virtual void visit(ast::VoidType& voidType) {assert(false);}
    virtual void visit(ast::IntegerType& integerType) {assert(false);}
    virtual void visit(ast::BoolType& boolType) {assert(false);}
    virtual void visit(ast::FloatingPointType& floatingPointType) {assert(false);}
    virtual void visit(ast::ArgumentDecl& argumentDecl) {assert(false);}
    virtual void visit(ast::FunctionDecl& functionDecl) {assert(false);}
    virtual void visit(ast::VariableDecl& variableDecl) {assert(false);}
    virtual void visit(ast::BinopExp& binOpExp) {assert(false);}
    virtual void visit(ast::FunctionCallExp& functionCallExp) {assert(false);}
    virtual void visit(ast::NumberLiteralExp& numberLiteralExp) {assert(false);}
    virtual void visit(ast::VariableExp& variableExp) {assert(false);}
    virtual void visit(ast::FunctionCalExplStmt& functionCallExpStmt) {assert(false);}
    virtual void visit(ast::ReturnStmt& returnStmt) {assert(false);}
    virtual void visit(ast::VariableDeclStmt& variableDeclStmt) {assert(false);}
    virtual void visit(ast::IfStmt& ifStmt) {assert(false);}
    virtual void visit(ast::BlockStmt& blockStmt) {assert(false);}
    virtual void visit(ast::Function& function) {assert(false);}
    virtual void visit(ast::Module& module) {assert(false);}
    virtual ~INodeVisitor() {}
};
