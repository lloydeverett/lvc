//
//  symbols.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/25.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <unordered_map>
#include <stack>
#include <cassert>
#include <string>

class Symbols {
private:
    std::unordered_map<std::string, ast::FunctionDecl> functions;
    std::unordered_map<std::string, std::stack<ast::VariableDecl>> vars;
    unsigned int currentScope;
    
public:
    
    void openScope() {
        currentScope++;
    }
    
    void destroySymbolsInCurrentScopeAndClose() {
        for ( dfiosiofjijofdsijodsf) {
            <#statements#>
        }
        currentScope--;
    }
};
