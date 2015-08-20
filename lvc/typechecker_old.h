//
//  semanticanalyzer.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/11.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <stdexcept>
#include "ast.h"
#include "symboltable.h"
#include "inodevisitor.h"

class TypeCheckerException : public std::exception {};
class TypeCheckerExceptionIncorrectReturnedExpressionType : public TypeCheckerException {};

//
// Because returning information from visitors is cumbersome
// (especially when dealing with functions that have differing
// return types), we let users of the type checker implement
// this interface. Note that the functions of the interface are called
// in a sane order: AST nodes are never received until their subnodes
// have been received.
//
template <typename VariableDeclAnnotations, typename FunctionDeclAnnotations>
class ITypeInformationReceiver {
protected:
    virtual void receiveArg(ast::CallExp& parentCallExp, ast::IExp& exp, ExpTypeInformation typeInformation) = 0;
    ITypeInformationReceiver();
public:
    virtual ~ITypeInformationReceiver();
};

class TypeCheckerExpVisitor : public INodeVisitor {
private:
    //
    // The target type indicates which expression type is desired.
    // For example, if we're checking the literal at the end of
    // char x = 3, then we're going to set the target type to char
    // and we're thus going to reason that the literal's type
    // is char, even though we'd reason that it's an int
    // were it not for this extra information (i.e. if targetType was not set).
    //
    // This happens when calling functions as well: the target
    // type is set to the type of the argument that appears in the
    // function's declaration.
    //
    const ast::IType* targetType;
    
public:
    TypeCheckerExpVisitor() : targetType(nullptr) {}
    void setTargetType(ast::IType* targetType) { this->targetType = targetType; }
    void setNoTargetType() { this->targetType = nullptr; }
    const ast::IType* getTargetType() { return targetType; }
    
    virtual void visit(ast::BinopExp& binOpExp) {
#WARNING TODO: TARGET TYPE
        setNoTargetType();
        binOpExp.lhs->accept(*this);
        binOpExp.rhs->accept(*this);
    }
    virtual void visit(ast::CallExp& callExp) {
        
    }
    virtual void visit(ast::NumberLiteralExp& numberLiteralExp) {
        
    }
    virtual void visit(ast::VariableExp& variableExp) {
        
    }
    virtual void visit(ast::CallExpStmt& callExpStmt) {
        
    }
};

template <typename VariableDeclAnnotations, typename FunctionDeclAnnotations>
class TypeCheckerStmtVisitor : public INodeVisitor {
private:
    TypeCheckerExpVisitor expVisitor;
    const ast::Function* currentFunction;
public:
    TypeCheckerStmtVisitor(SymbolTable<VariableDeclAnnotations, FunctionDeclAnnotations>& symbolTable);
    
    TypeCheckerStmtVisitor(const ast::Function* currentFunction) : currentFunction(currentFunction) {}
    void setCurrentFunction(const ast::Function* currentFunction) { this->currentFunction = currentFunction; }
    const ast::Function* getCurrentFunction() { return this->currentFunction; }
    
    virtual void visit(ast::CallExpStmt& callExpStmt) {
        expVisitor.setNoTargetType();
        callExpStmt.callExp.accept(expVisitor);
    }
    virtual void visit(ast::ReturnStmt& returnStmt) {
        assert(currentFunction != nullptr);
        if (returnStmt.optReturnedExpression) {
            ast::IExp& returnedExpression = *(*returnStmt.optReturnedExpression).get();
            expVisitor.setTargetType(currentFunction->decl.returnType.get());
            returnedExpression.accept(expVisitor);
        }
        else {
            if (currentFunction->decl.returnType->typeIndex() != typeid(ast::VoidType)) {
                throw TypeCheckerExceptionIncorrectReturnedExpressionType();
            }
        }
    }
    virtual void visit(ast::VariableDeclStmt& variableDeclStmt) {
        if (variableDeclStmt.optInit) {
            expVisitor.setTargetType(variableDeclStmt.decl.type.get());
            
        }
    }
    virtual void visit(ast::IfStmt& ifStmt) {
        expVisitor.setTargetType(ast::BoolType());
        ifStmt.condition->accept(expVisitor);
        
    }
    virtual void visit(ast::BlockStmt& blockStmt) {
        for (std::unique_ptr<ast::IStmt>& statement : blockStmt.statements) {
            statement->accept(*this);
        }
    }
};
