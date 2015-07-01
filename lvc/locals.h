//
//  locals.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/30.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <unordered_map>
#include <stack>
#include <cassert>

class Locals {
private:
    std::unordered_map<std::string, std::stack<ast::VariableDecl*>> locals;
    
    unsigned int scope;
    
public:
    void openScope() {
        scope++;
    }
    
    void add() {
        
    }
    
    void closeScope() {
        assert(scope > 0);
    }
    
    void clear() {
    }
};
