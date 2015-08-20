//
//  symboltable.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/11.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <stdexcept>
#include <cassert>
#include <string>
#include <unordered_map>
#include <stack>
#include <utility>
#include "symboltableexceptions.h"
#include "ast.h"

class SymbolTable {
private:
    unsigned int currentVarScope;
    struct VarSymbolsStackElem {
        unsigned int scope;
        ast::VariableDecl* declPtr;
    };
    std::unordered_map<std::string, std::stack<VarSymbolsStackElem>> varSymbols;
    std::unordered_map<std::string, ast::FunctionDecl*> fnSymbols;
    
public:
    SymbolTable() : currentVarScope(0) {}
    
    void openVariableScope() {
        currentVarScope++;
    }
    void closeVariableScope() {
        assert(currentVarScope > 0);
        currentVarScope--;
    }
    void registerVariable(ast::VariableDecl* v) {
        auto& stack = varSymbols[v->identifier];
        assert(stack.empty() || stack.top().scope <= currentVarScope);
        if (!stack.empty() && stack.top().scope == currentVarScope) {
            throw SymbolTableExceptionVariableAlreadyRegisteredInScope();
        }
        stack.push(VarSymbolsStackElem{currentVarScope, v});
    }
    void registerFunction(ast::FunctionDecl* f) {
        auto pair = fnSymbols.insert(std::unordered_map<std::string, ast::FunctionDecl*>::value_type(f->identifier, f));
        if (!pair.second) {
            throw SymbolTableExceptionFunctionAlreadyRegistered();
        }
    }
    ast::VariableDecl* lookupVariable(const std::string& identifier) {
        auto iterator = varSymbols.find(identifier);
        if (iterator == varSymbols.end()) {
            throw SymbolTableExceptionVariableNotFoundUponLookup();
        }
        auto& stack = iterator->second;
        if (stack.empty()) {
            throw SymbolTableExceptionFunctionNotFoundUponLookup();
        }
        return stack.top().declPtr;
    }
    ast::FunctionDecl* lookupFunction(const std::string& identifier) {
        auto iterator = fnSymbols.find(identifier);
        if (iterator == fnSymbols.end()) {
            throw SymbolTableExceptionFunctionNotFoundUponLookup();
        }
        return iterator->second;
    }
};
