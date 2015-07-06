//
//  semanticanalyzer.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/30.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "ast.h"
#include "inodevisitor.h"
#include "collectedsymbols.h"
#include "iissuereporter.h"
#include <unordered_map>
#include <stack>

struct IdentifierMeaning {
    const unsigned int scope;
    ast::IDecl* const refersTo;
    IdentifierMeaning(unsigned int scope, ast::IDecl* refersTo) : scope(scope), refersTo(refersTo) {}
};
    
class SemanticAnalyzerVisitor : public INodeVisitor {
    std::unordered_map<std::string, std::stack<IdentifierMeaning>> locals;
    CollectedSymbols& collectedSymbols;
    IIssueReporter& issueReporter;
    
public:
    SemanticAnalyzerVisitor(CollectedSymbols& collectedSymbols, IIssueReporter& issueReporter) :
    collectedSymbols(collectedSymbols), issueReporter(issueReporter) {}
    
    virtual void visit(ast::BinopExp& binOpExp) {
        binOpExp.lhs->accept(*this);
        binOpExp.rhs->accept(*this);
        
        
    }
    virtual void visit(ast::FunctionCallExp& functionCallExp) {
        for (std::unique_ptr<ast::IExp> arg : functionCallExp.passedArguments) {
            arg->accept(*this);
        }
        
        
    }
    
    virtual void visit(ast::NumberLiteralExp& numberLiteralExp) {assert(false);}
    virtual void visit(ast::VariableExp& variableExp) {assert(false);}
    virtual void visit(ast::FunctionCalExplStmt& functionCallExpStmt) {assert(false);}
    virtual void visit(ast::ReturnStmt& returnStmt) {assert(false);}
    virtual void visit(ast::VariableDeclStmt& variableDeclStmt) {assert(false);}
    virtual void visit(ast::IfStmt& ifStmt) {assert(false);}
    virtual void visit(ast::BlockStmt& blockStmt) {assert(false);}
    virtual void visit(ast::Function& function) {assert(false);}
    virtual void visit(ast::Module& module) {assert(false);}
};
