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

void ArgumentDecl::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void FunctionDecl::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void VariableDecl::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void NumberLiteralExp::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void BinopExp::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void FunctionCallExp::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void VariableExp::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void FunctionCalExplStmt::accept(INodeVisitor& visitor) {
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

void Function::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}

void Module::accept(INodeVisitor& visitor) {
    visitor.visit(*this);
}
