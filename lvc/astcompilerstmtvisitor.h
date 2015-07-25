//
//  astcompilerstmtvisitor.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/16.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <cassert>
#include "ast.h"
#include "inodevisitor.h"
#include "symboltable.h"
#include "astcompilationconfig.h"
#include "astcompilerexpvisitor.h"
#include "astcompilerexceptions.h"

template <typename Generator>
class ASTCompiler;

template <typename Generator>
class ASTCompilerStmtVisitor : public INodeVisitor {
    friend class ASTCompiler<Generator>;
private:
    typedef typename Generator::VariableDeclAnnotations VariableDeclAnnotations;
    typedef typename Generator::FunctionDeclAnnotations FunctionDeclAnnotations;
    typedef typename Generator::Temporary Temporary;
    typedef typename Generator::WithoutElseIfRep WithoutElseIfRep;
    typedef typename Generator::WithElseIfRep WithElseIfRep;
    ast::Function* currentFunctionPtr;
    Generator& generator;
    SymbolTable<VariableDeclAnnotations, FunctionDeclAnnotations>& symbolTable;
    ASTCompilationConfig& config;
    
    ASTCompilerExpVisitor<Generator> expVisitor;
protected:
    ASTCompilerStmtVisitor(ASTCompilationConfig& config, Generator &generator,
                           SymbolTable<VariableDeclAnnotations, FunctionDeclAnnotations>& symbolTable)
    : config(config), generator(generator), symbolTable(symbolTable), currentFunctionPtr(nullptr), expVisitor(config, generator, symbolTable) {}
public:
    void compileFunction(ast::Function& function) {
        assert(currentFunctionPtr == nullptr);
        currentFunctionPtr = &function;
        generator.beginFunctionDefinition(function.decl);
        symbolTable.openVariableScope();
   //     for (ast::ArgumentDecl& argDecl : function.decl.arguments)
   //         symbolTable.registerDecl(
        performVisitOnBlockWithoutOpeningScope(function.block);
        symbolTable.closeVariableScope();
        if (!generator.isCurrentBlockTerminated()) {
            if (dynamic_cast<ast::VoidType*>(function.decl.returnType.get()) != nullptr) {
                generator.genVoidReturn();
            }
            else {
                throw ASTCompilerExceptionNoReturnValue();
            }
        }
        generator.endFunctionDefinition();
        currentFunctionPtr = nullptr;
    }
    virtual void visit(ast::CallExpStmt& callExpStmt) {
        expVisitor.performVisit(callExpStmt.callExp, nullptr);
    }
    virtual void visit(ast::ReturnStmt& returnStmt) {
        if (returnStmt.optReturnedExpression) {
            expVisitor.performVisit(*returnStmt.optReturnedExpression, &currentFunctionPtr->decl.getType());
            generator.genReturn(expVisitor.val());
        }
        else {
            if (dynamic_cast<ast::VoidType*>(currentFunctionPtr->decl.returnType.get()) == nullptr)
                throw ASTCompilerExceptionNoReturnValue();
            generator.genVoidReturn();
        }
    }
    virtual void visit(ast::VariableDeclStmt& variableDeclStmt) {
        if (variableDeclStmt.optInitialValue) {
            expVisitor.performVisit(*variableDeclStmt.optInitialValue, variableDeclStmt.decl.type.get());
            symbolTable.registerDecl(&variableDeclStmt.decl, generator.genLocalWithInit(variableDeclStmt.decl, expVisitor.val()));
        }
        else {
            symbolTable.registerDecl(&variableDeclStmt.decl, generator.genLocalWithoutInit(variableDeclStmt.decl));
        }
    }
    virtual void visit(ast::IfStmt& ifStmt) {
        ast::BoolType boolType(Constant);
        expVisitor.performVisit(ifStmt.condition, &boolType);
        if (!ifStmt.elseBlock) {
            // If statement doesn't have an else
            WithoutElseIfRep rep = generator.startWithoutElseIf(expVisitor.val());
            performVisit(ifStmt.thenBlock);
            generator.endWithoutElseIf(rep);
        }
        else {
            // If statement has an else
            WithElseIfRep rep = generator.startWithElseIf(expVisitor.val());
            performVisit(ifStmt.thenBlock);
            generator.endThenStartElse(rep);
            performVisit(*ifStmt.elseBlock);
            generator.endWithElseIf(rep);
        }
    }
    virtual void visit(ast::BlockStmt& blockStmt) {
        symbolTable.openVariableScope();
        performVisitOnBlockWithoutOpeningScope(blockStmt);
        symbolTable.closeVariableScope();
    }
    void performVisitOnBlockWithoutOpeningScope(ast::BlockStmt& blockStmt) {
        for (std::unique_ptr<ast::IStmt>& stmt : blockStmt.statements)
            performVisit(stmt);
    }
    void performVisit(ast::IStmt& stmt) {
        stmt.accept(*this);
    }
    void performVisit(std::unique_ptr<ast::IStmt>& stmt) {
        performVisit(*stmt);
    }
};
