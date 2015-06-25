//
//  symbols.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/19.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <vector>
#include <unordered_map>
#include <cassert>
#include <boost/optional.hpp>
#include <stdexcept>
#include "ast.h"

#warning TODO: determine whether to use map or unordered_map
#warning TODO: consider using stacks

class CouldNotAssignTBDException : std::exception {
private:
    std::string identifier;
public:
    CouldNotAssignTBDException(std::string identifier) : identifier(identifier) {}
    std::string getIdentifier() {
        return identifier;
    }
};

class Symbols {
private:
    std::vector<std::unordered_map<std::string, ast::IDecl*>> symbolVec;
    std::unordered_map<std::string, std::vector<ast::IDecl**>> undefineds;
    
public:
    Symbols() {
        symbolVec.push_back(std::unordered_map<std::string, ast::IDecl*>());
    }
    
    void openScope() {
        symbolVec.push_back(std::unordered_map<std::string, ast::IDecl*>());
    }
    
    void closeScope() {
        assert(symbolVec.size() > 0);
        symbolVec.pop_back();
    }
    
    void create(ast::IDecl* decl) {
        assert(symbolVec.size() > 0);
        symbolVec[symbolVec.size() - 1].insert(std::pair<std::string, ast::IDecl*>(decl->getIdentifier(), decl));
    }
    
    boost::optional<ast::IDecl*> getSymbol(std::string identifier) {
        // Iterate backwards over the vector elements (maps)
        for (auto rIt = symbolVec.rbegin(); rIt != symbolVec.rend(); ++rIt) {
            const auto& theMap = *rIt;
            const auto iteratorToElement = theMap.find(identifier);
            if (iteratorToElement != theMap.end()) {
                return iteratorToElement->second;
            }
        }
        return boost::none;
    }
    
    ast::IDecl* getSymbolElseAddToUndefineds(std::string identifier, ast::IDecl** declPtrPtr) {
        boost::optional<ast::IDecl*> optSymbol = getSymbol(identifier);
        if (optSymbol) {
            return *optSymbol;
        }
        else {
            undefineds[identifier].push_back(declPtrPtr);
            return nullptr;
        }
    }
    
    void defineUndefinedsUsingCurrentlyStoredSymbols() {
        for (auto iter : undefineds) {
            const std::string& identifier = iter.first;
            const std::vector<ast::IDecl**> undefinedVec = iter.second;
            
            boost::optional<ast::IDecl*> declPtrOpt = getSymbol(identifier);
            if (!declPtrOpt) {
                throw CouldNotAssignTBDException(identifier);
            }
            ast::IDecl* declPtr = *declPtrOpt;
            
            for (ast::IDecl** thisUndefined : undefinedVec) {
                *thisUndefined = declPtr;
            }
        }
        undefineds.clear();
    }
};
