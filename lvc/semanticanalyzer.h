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
#include "iissuereporter.h"
#include <unordered_map>
#include <stack>
#include <stdexcept>
#include <cassert>
#include <vector>

/*
 
 The semantic analyzer takes a full AST module in its constructor and a reference to an IAnnotationFetcher.
 
 Upon construction, it looks through the module and stores symbols that can be used before they're
 declared in a map.
 
 The semantic analyzer can then be called to analyze parts of the AST. In doing so, it associates
 identifiers with the declarations to which they refer.
 
 The class allows its user to associate particular annotations with each declaration (the type of which
 is also specified by the user through templates). This can be used for e.g. IR generation.
 
 */

class SemanticAnalyzerException : public std::exception {};
class SemanticAnalyzerExceptionVariableAlreadyExistsInTargetScope : public SemanticAnalyzerException {};
class SemanticAnalyzerExceptionFunctionAlreadyExists : public SemanticAnalyzerException {};
class SemanticAnalyzerExceptionSymbolDoesNotExistUponLookup : public SemanticAnalyzerException {};
class SemanticAnalyzerExceptionFunctionCallSignatureMismatch : public SemanticAnalyzerException {};

template <typename DeclAnnotation>
class SemanticAnalyzer {
private:
    struct VariableIdentifierMeaning {
        unsigned int scope;
        const ast::VariableDecl* declPtr;
        DeclAnnotation annotation;
    };
    struct FunctionIdentifierMeaning {
        const ast::FunctionDecl* declPtr;
        DeclAnnotation annotation;
    };
    
    // These keep the symbols that are currently available to the code we're currently analyzing.
    // They hold all the globals & functions (which are always at the top level of the stack),
    // and variableSymbols contains the locals that are currently available.
    std::unordered_map<std::string, std::stack<VariableIdentifierMeaning>> variableSymbols;
    std::unordered_map<std::string, std::stack<FunctionIdentifierMeaning>> functionSymbols;
    
    unsigned int currentScope;
    
    void clearVariablesAtScopeOrAtDeeperScope(unsigned int scope) {
        for (auto& stack : variableSymbols) {
            while (!stack.empty() && stack.top().scope >= scope) {
                stack.pop();
            }
        }
    }
    VariableIdentifierMeaning lookupVariable(const std::string& identifier) {
        auto iterator = variableSymbols.find(identifier);
        if (iterator == variableSymbols.end())
            throw SemanticAnalyzerExceptionSymbolDoesNotExistUponLookup();
        return iterator.second;
    }
    FunctionIdentifierMeaning lookupFunction(const std::string& identifier) {
        auto iterator = functionSymbols.find(identifier);
        if (iterator == functionSymbols.end())
            throw SemanticAnalyzerExceptionSymbolDoesNotExistUponLookup();
        return iterator.second;
    }
public:
    SemanticAnalyzer() : currentScope(0) {}
    
    ////////////////////////////////////
    
    void acknowledgeFunctionDecl(const ast::FunctionDecl* decl, DeclAnnotation annotation) {
        auto& stack = functionSymbols[declPtr->getIdentifier()];
        if (!stack.empty()) {
            throw SemanticAnalyzerExceptionFunctionAlreadyExists();
        }
        stack.push(FunctionIdentifierMeaning{declPtr, annotation});
    }
    void acknowledgeVariableDecl(const ast::VariableDecl* decl, DeclAnnotation annotation) {
        auto& stack = variableSymbols[declPtr->getIdentifier()];
        if (!stack.empty() && stack.top().scope == currentScope) {
            throw SemanticAnalyzerExceptionVariableAlreadyExistsInTargetScope();
        }
        stack.push(VariableIdentifierMeaning{currentScope, declPtr, annotation});
    }
    void openScope() {
        currentScope++;
    }
    void closeScope() {
        assert(currentScope > 0);
        clearVariablesAtScopeOrAtDeeperScope(currentScope);
        currentScope--;
    }
    
    ////////////////////////////////////
    
    class ExpAnalysis {
    protected:
        ExpAnalysis();
    public:
        virtual ~ExpAnalysis() {};
        virtual const ast::IType& getType() = 0;
    };
    
    struct IdentifierUsageExpAnalysis : public ExpAnalysis {
        const ast::IDecl* declPtr;
        DeclAnnotation declAnnotation; // a copy of the annotation of the decl
        virtual const ast::IType& getType() {
            return declPtr->getType();
        }
    };
    IdentifierUsageExpAnalysis analyzeVariableExp(ast::VariableExp& variableExp) {
        VariableIdentifierMeaning meaning = lookupVariable(variableExp.identifier);
        return IdentifierUsageExpAnalysis{meaning.declPtr, meaning.annotation};
    }
    IdentifierUsageExpAnalysis analyzeFunctionCallExp(const ast::FunctionCallExp& functionCallExp,
                                                      std::vector<ExpAnalysis*> argumentAnalyses) {
        assert(functionCallExp.passedArguments.size() == argumentAnalyses.size());
        
        FunctionIdentifierMeaning calleeMeaning = lookupVariable(functionCallExp.calleeIdentifier);
        
        // Check that the number of passed arguments is the same as the number of arguments
        // in the function's declaration.
        if (calleeMeaning.declPtr->arguments.size() != argumentAnalyses.size())
            throw SemanticAnalyzerExceptionFunctionCallSignatureMismatch();
        
        for (size_t i = 0; i < argumentAnalyses.size(); i++) {
            const ExpAnalysis& expAnalysis = *argumentAnalyses[i];
            const ast::IDecl& decl = calleeMeaning.declPtr->arguments[i];
            
            // Check that the types match up.
            if (expAnalysis.getType() != decl.getType()) {
                throw SemanticAnalyzerExceptionFunctionCallSignatureMismatch();
            }
        }
        
        return IdentifierUsageExpAnalysis{calleeMeaning.declPtr, calleeMeaning.annotation};
    }
};
