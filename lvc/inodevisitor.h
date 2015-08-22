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
    //
    // The assertions allow implementations to only implement visit functions for
    // a particular subset of the node subclasses in the AST.
    //
    // If an assert in this file fails, the visitor does
    // not implement a visit function for the AST class you're trying to visit.
    //
    virtual void visit(ast::VoidType& voidType) {assert(false);}
    virtual void visit(ast::IntegerType& integerType) {assert(false);}
    virtual void visit(ast::BoolType& boolType) {assert(false);}
    virtual void visit(ast::FloatingPointType& floatingPointType) {assert(false);};
    virtual void visit(ast::NonArgVariableDecl& nonArgVariableDecl) {assert(false);};
    virtual void visit(ast::ArgVariableDecl& argVariableDecl) {assert(false);};
    virtual void visit(ast::FunctionDecl& functionDecl) {assert(false);}
    virtual void visit(ast::BinopExp& binopExp) {assert(false);}
    virtual void visit(ast::CallExp& callExp) {assert(false);}
    virtual void visit(ast::NumberLiteralExp& numberLiteralExp) {assert(false);}
    virtual void visit(ast::VariableExp& variableExp) {assert(false);}
    virtual void visit(ast::CallExpStmt& callExpStmt) {assert(false);}
    virtual void visit(ast::ReturnStmt& returnStmt) {assert(false);}
    virtual void visit(ast::VariableDeclStmt& variableDeclStmt) {assert(false);}
    virtual void visit(ast::IfStmt& ifStmt) {assert(false);}
    virtual void visit(ast::BlockStmt& blockStmt) {assert(false);}
    virtual void visit(ast::IncrementStmt& incrementStmt) {assert(false);}
    virtual void visit(ast::DecrementStmt& decrementStmt) {assert(false);}
    virtual void visit(ast::VarBinopStmt& VarBinopStmt) {assert(false);}
    virtual void visit(ast::AssignStmt& assignStmt) {assert(false);}
    virtual void visit(ast::Function& function) {assert(false);}
    virtual void visit(ast::Module& module) {assert(false);}
    
    virtual ~INodeVisitor() {}
};
