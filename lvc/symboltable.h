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
#include "ast.h"

class SymbolTableException : public std::exception {};
class SymbolTableExceptionSymbolAlreadyRegisteredInScope : public SymbolTableException {};
class SymbolTableExceptionFunctionAlreadyRegistered : public  SymbolTableException {};
class SymbolTableExceptionVariableNotFoundUponLookup : public SymbolTableException {};
class SymbolTableExceptionFunctionNotFoundUponLookup : public SymbolTableException {};

//
// The SymbolTable class allows its user to associate annotations with each symbol that
// is registered in the table. Then, when the user looks up an identifier, the annotations
// associated with the declaration that the identifier refers to and a pointer to the declaration are retrieved.
//
template <typename VariableDeclAnnotations, typename FunctionDeclAnnotations>
class SymbolTable {
private:
    struct IdentifierVariableAssociation {
        unsigned int declScope;
        const ast::VariableDecl* declPtr;
        VariableDeclAnnotations annotations;
    };
    struct IdentifierFunctionAssociation {
        const ast::FunctionDecl* declPtr;
        FunctionDeclAnnotations annotations;
    };
    
    unsigned int currentVariableScope;
    std::unordered_map<std::string, std::stack<IdentifierVariableAssociation>> variableSymbols;
    std::unordered_map<std::string, IdentifierFunctionAssociation> functionSymbols;
    
    void destroyVariablesInScope(unsigned int scope) {
        for (auto& pair : variableSymbols) {
            auto& stack = pair.second;
            while (!stack.empty() && stack.top().declScope == scope) {
                stack.pop();
            }
        }
    }
public:
    SymbolTable() : currentVariableScope(0) {}
    void registerDecl(const ast::VariableDecl* variableDecl, VariableDeclAnnotations annotations) {
        auto& stack = variableSymbols[variableDecl->getIdentifier()];
        assert(stack.empty() || stack.top().declScope <= currentVariableScope);
        if (!stack.empty() && stack.top().declScope == currentVariableScope) {
            throw SymbolTableExceptionSymbolAlreadyRegisteredInScope();
        }
        stack.push(IdentifierVariableAssociation{currentVariableScope, variableDecl, annotations});
    }
    void registerDecl(const ast::FunctionDecl* functionDecl, FunctionDeclAnnotations annotations) {
        auto pair = functionSymbols.insert(decltype(functionSymbols)::value_type(functionDecl->getIdentifier(),
                                                                                 IdentifierFunctionAssociation{functionDecl}, annotations));
        if (!pair.second) {
            throw SymbolTableExceptionFunctionAlreadyRegistered();
        }
    }
    void openVariableScope() {
        currentVariableScope++;
    }
    void closeVariableScope() {
        assert(currentVariableScope > 0);
        destroyVariablesInScope(currentVariableScope);
        currentVariableScope--;
    }
    std::pair<VariableDeclAnnotations, const ast::VariableDecl*> lookupVariable(const std::string& identifier) {
        auto iterator = variableSymbols.find(identifier);
        if (iterator == variableSymbols.end()) {
            throw SymbolTableExceptionVariableNotFoundUponLookup();
        }
        auto association = iterator.second;
        return std::pair<VariableDeclAnnotations, const ast::VariableDecl*>(association.annotations, association.declPtr);
    }
    std::pair<FunctionDeclAnnotations, const ast::FunctionDecl*> lookupFunction(const std::string& identifier) {
        auto iterator = functionSymbols.find(identifier);
        if (iterator == functionSymbols.end()) {
            throw SymbolTableExceptionFunctionNotFoundUponLookup();
        }
        auto association = iterator.second;
        return std::pair<FunctionDeclAnnotations, const ast::FunctionDecl*>(association.annotations, association.declPtr);
    }
};
