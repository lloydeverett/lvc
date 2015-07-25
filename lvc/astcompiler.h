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
#include "astcompilerstmtvisitor.h"
#include "astcompilationconfig.h"

#warning TODO: check non void variable decl
#warning TODO: type folding
#warning TODO: Revise this list
#warning TODO: binopexp only defined for numeral
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
class ASTCompiler {
private:
    typedef typename Generator::VariableDeclAnnotations VariableDeclAnnotations;
    typedef typename Generator::FunctionDeclAnnotations FunctionDeclAnnotations;
    typedef typename Generator::Temporary Temporary;
    ASTCompilationConfig config;
    SymbolTable<VariableDeclAnnotations, FunctionDeclAnnotations> symbolTable;
    Generator generator;
    
    ASTCompilerStmtVisitor<Generator> stmtVisitor;
    
public:
    ASTCompiler(ASTCompilationConfig config, typename Generator::Config genConfig) :
    config(config), generator(genConfig), stmtVisitor(this->config, generator, symbolTable) {}
    
    void compileVariableDecls(ast::Module* module) {
        for (ast::VariableDeclStmt& declStmt : module->globals) {
            
        }
    }
    
    void compileFunctionDecls(ast::Module* module) {
        for (ast::Function& function : module->functions) {
            ast::FunctionDecl& decl = function.decl;
            symbolTable.registerDecl(&decl, generator.genFunctionDecl(decl));
        }
    }
    
    void compileFunctionDefinitions(ast::Module* module) {
        for (ast::Function& function : module->functions) {
            stmtVisitor.compileFunction(function);
        }
    }
};
