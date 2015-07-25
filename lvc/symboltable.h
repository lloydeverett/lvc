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

struct VariableSymbolBase {
    ast::VariableDecl* declPtr;
    const std::string& getIdentifier() {
        return declPtr->getIdentifier();
    }
};

struct FunctionSymbolBase {
    ast::FunctionDecl* declPtr;
    const std::string& getIdentifier() {
        return declPtr->getIdentifier();
    }
};

template <typename VariableSymbol, typename FunctionSymbol>
class SymbolTable {
private:
    unsigned int currentVarScope;
    struct VarSymbolsStackElem {
        unsigned int scope;
        VariableSymbol symbol;
    };
    std::unordered_map<std::string, std::stack<VarSymbolsStackElem>> varSymbols;
    std::unordered_map<std::string, FunctionSymbol> fnSymbols;
    
public:
    SymbolTable() : currentVarScope(0) {}
    
    void openVarScope() {
        currentVarScope++;
    }
    void closeVarScope() {
        assert(currentVarScope > 0);
        currentVarScope--;
    }
    void registerVar(VariableSymbol v) {
        auto& stack = varSymbols[v.getIdentifier()];
        assert(stack.empty() || stack.top().scope <= currentVarScope);
        if (stack.top().scope == currentVarScope) {
            throw SymbolTableExceptionVariableAlreadyRegisteredInScope();
        }
        stack.push(VarSymbolsStackElem{currentVarScope, v});
    }
    void registerFn(FunctionSymbol fn) {
        auto pair = fnSymbols.insert(std::pair<const std::string, FunctionSymbol>(fn.getIdentifier(), fn));
        if (!pair.second) {
            throw SymbolTableExceptionFunctionAlreadyRegistered();
        }
    }
    VariableSymbol lookupVar(const std::string& identifier) {
        auto iterator = varSymbols.find(identifier);
        if (iterator == varSymbols.end()) {
            throw SymbolTableExceptionVariableNotFoundUponLookup();
        }
        auto& stack = iterator->second;
        if (stack.empty()) {
            throw SymbolTableExceptionVariableNotFoundUponLookup();
        }
        return stack.top();
    }
    FunctionSymbol lookupFn(const std::string& identifier) {
        auto iterator = fnSymbols.find(identifier);
        if (iterator == fnSymbols.end()) {
            throw SymbolTableExceptionFunctionNotFoundUponLookup();
        }
        return iterator->second;
    }
};
