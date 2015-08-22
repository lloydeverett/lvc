//
//  ast.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/13.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "ast.h"
#include "inodevisitor.h"

using namespace ast;

void VoidType::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void IntegerType::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void BoolType::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void FloatingPointType::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void NonArgVariableDecl::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void ArgVariableDecl::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void FunctionDecl::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void NumberLiteralExp::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void BinopExp::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void CallExp::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void VariableExp::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void CallExpStmt::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void ReturnStmt::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void VariableDeclStmt::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void IfStmt::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void BlockStmt::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void IncrementStmt::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void DecrementStmt::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void VarBinopStmt::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void AssignStmt::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void Function::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void Module::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}
