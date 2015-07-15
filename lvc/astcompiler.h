//
//  astcompiler.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/14.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <memory>
#include "ast.h"
#include "symboltable.h"
#include "typechecker.h"
#include "inodevisitor.h"

#warning TODO: check non void variabled ecl

#warning TODO: Revise this list
//
// IR Generator classes must:
//  - Make all of the following resolve to a type:
//     * Generator::VariableDeclAnnotations
//     * Generator::FunctionDeclAnnotations
//     * Generator::Config
//  - Implement the following functions:
//     * void setConfig(Generator::Config)
//     * void doNonLocalsSymbolGen(const ast::Module& module)
//

template <typename Generator>
class ASTCompiler;

template <typename Generator>
class ASTCompilerExpVisitor : public INodeVisitor {
    friend class ASTCompiler<Generator>;
protected:
    ASTCompilerExpVisitor();
public:
    virtual void visit(ast::BinopExp& binOpExp) {
        
    }
    virtual void visit(ast::CallExp& callExp) {
        
    }
    virtual void visit(ast::NumberLiteralExp& numberLiteralExp) {
        
    }
    virtual void visit(ast::VariableExp& variableExp) {
        
    }
};

template <typename Generator>
class ASTCompilerStmtVisitor : public INodeVisitor {
    friend class ASTCompiler<Generator>;
protected:
    ASTCompilerStmtVisitor();
public:
    virtual void visit(ast::CallExpStmt& callExpStmt) {
        
    }
    virtual void visit(ast::ReturnStmt& returnStmt) {
        
    }
    virtual void visit(ast::VariableDeclStmt& variableDeclStmt) {
        
    }
    virtual void visit(ast::IfStmt& ifStmt) {
        
    }
    virtual void visit(ast::BlockStmt& blockStmt) {
        
    }
};

template <typename Generator>
class ASTCompiler {
private:
    typedef typename Generator::VariableDeclAnnotations VariableDeclAnnotations;
    typedef typename Generator::FunctionDeclAnnotations FunctionDeclAnnotations;
    
    // Note that the order of these members is important.
    SymbolTable<VariableDeclAnnotations, FunctionDeclAnnotations> symbolTable;
    Generator generator;
    TypeChecker typeChecker;
    
    ASTCompilerStmtVisitor<Generator> stmtVisitor;
    
public:
    ASTCompiler(typename Generator::Config genConfig) : generator(genConfig) {}
    
    void compileNonLocalSymbols(ast::Module& module) {
        for (ast::VariableDeclStmt& globalDeclStmt : module.globals) {
            VariableDeclAnnotations annotations;
            
            // Check that we can make an expression of globalDeclStmt.decl.type
            typeChecker.check(, <#ast::IType &targetType#>)
            
            // Gen the symbol
            annotations = generator.genGlobal(globalDeclStmt);
            
            // Add it to the symbol table
            symbolTable.registerDecl(globalDeclStmt.decl, annotations);
        }
        for (ast::Function& function : module.functions) {
            ast::FunctionDecl& decl = function.decl;
            
            FunctionDeclAnnotations annotations;
            
        }
    }
    
    void compileFunctionDefinitions(ast::Module& module) {
        // Iterate over each function, type checking each one and running it by the generator.
        for (ast::Function& function : module.functions) {
            ast::BlockStmt& block = function.block;
            
            for (std::unique_ptr<ast::IStmt>& stmt : block.statements) {
                stmt->accept(stmtVisitor);
            }
        }
    }
};
