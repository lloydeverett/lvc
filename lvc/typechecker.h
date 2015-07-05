//
//  typechecker.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/30.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "ast.h"
#include <unordered_map>
#include <stack>

struct IdentifierMeaning {
    const unsigned int scope;
    ast::IDecl* const refersTo;
    IdentifierMeaning(unsigned int scope, ast::IDecl* refersTo) : scope(scope), refersTo(refersTo) {}
};

class TypeChecker {
    ast::Module& module;
    CollectedSymbols& collectedSymbols;
    std::unordered_map<std::string, std::stack<IdentifierMeaning>> locals;
    
public:
    TypeChecker(ast::Module& module, CollectedSymbols& collectedSymbols) : module(module), collectedSymbols(collectedSymbols) {}
    
    
};
    
class TypeCheckerVisitor : public INodeVisitor {
    std::unordered_map<std::string, std::stack<IdentifierMeaning>> locals;
    CollectedSymbols& collectedSymbols;
    
    
};

// Collected symbols are symbols that we 'collected'
// during the first parse
class CollectedSymbols {
    
};
