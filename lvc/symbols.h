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
#include <cstdint>
#include <stdexcept>
#include <boost/optional.hpp>
#include "ast.h"

class SymbolsException : std::exception {};
class SymbolsExceptionVarAlreadyExistsInScope : SymbolsException {};
class SymbolsExceptionVarDoesNotExist : SymbolsException {};

class Symbols {
private:
    std::unordered_map<std::string, std::stack<ast::VariableDecl*>> vars;
    std::unordered_map<std::string, std::vector<ast::VariableDecl**>> undefinedVars;
    std::unordered_map<std::string, ast::FunctionDecl*> functions;
    uint64_t currentScope;
    
    template<class T>
    uint64_t indexOfTopInStack(const std::stack<T>& s) {
        assert(s.size() > 0);
        return s.size() - 1;
    }
    
public:
    
    Symbols() {
        currentScope = 0;
    }
    
    void openVarScope() {
        currentScope++;
    }
    
    boost::optional<ast::VariableDecl*> tryGetVar(const std::string& identifier) {
        const auto iter = vars.find(identifier);
        if (iter == vars.end()) {
            return boost::none;
        }
        const auto& stack = iter->second;
        if (stack.empty()) {
            return boost::none;
        }
        return stack.top();
    }
    
    void tryGetVarElseAddToUndefineds(const std::string& identifier, ast::VariableDecl** declPtrPtr) {
        boost::optional<ast::VariableDecl*> varPtrOpt = tryGetVar(identifier);
        if (varPtrOpt) {
            ast::VariableDecl* varPtr = *varPtrOpt;
            *declPtrPtr = varPtr;
        }
        else {
            *declPtrPtr = nullptr;
            undefinedVars[identifier].push_back(declPtrPtr);
        }
    }
    
    void addVar(ast::VariableDecl* declPtr) {
        auto& stack = vars[declPtr->identifier];
        
        // Check that the variable isn't already in the current scope
        if (!stack.empty() && indexOfTopInStack(stack) == currentScope) {
            throw SymbolsExceptionVarAlreadyExistsInScope();
        }
        
        stack.push(declPtr);
    }
    
    void destroyVarsInCurrentScope() {
        for (auto& pair : vars) {
            auto& stack = pair.second;
            
            if (stack.empty()) continue;
            
            uint64_t stackScope = indexOfTopInStack(stack);
            assert(stackScope <= currentScope);
            if (stackScope == currentScope) {
                stack.pop();
            }
        }
    }
    
    void closeVarScope() {
        assert(currentScope > 0);
        destroyVarsInCurrentScope();
        currentScope--;
    }
    
};
