//
//  compiler.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/26.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "semanticanalyzer.h"

template <typename IRGeneratorT>
class Compiler {
    SemanticAnalyzer& semanticAnalyzer;
    IRGeneratorT& generator;
    
public:
    Compiler(SemanticAnalyzer& semanticAnalyzer, IRGeneratorT& generator) : semanticAnalyzer(semanticAnalyzer), generator(generator) {}
    
    void compileLookaheadSymbols(ast::Module& module) {
        // Add to symbol table
        for (ast::VariableDeclStmt& declStmt : module.globals) {
            semanticAnalyzer.addLookaheadGlobalDecl(declStmt.decl);
            generator.genLookaheadGlobal(declStmt.decl);
        }
        for (ast::Function& fn : module.functions) {
            semanticAnalyzer.addLookaheadFunctionDecl(fn.decl);
            generator.genLookaheadFunctionDecl(fn.decl);
        }
    }
    
    void compileDefinitions(ast::Module& module) {
        // Set global initializers
        for (ast::VariableDeclStmt& declStmt : module.globals) {
            // Generate it with a constant folder
        }
        
        // Compile function definitions
        for (ast::Function& fn : module.functions) {
            semanticAnalyzer.analyzeFunctionDefinition(fn);
            generator.genFunctionDefinition(fn);
        }
    }
};
