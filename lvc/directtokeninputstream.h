//
//  directtokeninputstream.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/05.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <queue>
#include "ilexer.h"
#include "itokeninputstream.h"

class DirectTokenInputStream : public ITokenInputStream {
private:
    Token next;
    Token nextNext;
    unsigned int aheadBy; // aheadBy can be from 0 (next is not filled) to 2 (next and nextNext are filled)
    ILexer& lexer;
    
    Token popNext() {
        assert(aheadBy > 0);
        Token t = next;
        next = nextNext;
        aheadBy--;
        return t;
    }
public:
    DirectTokenInputStream(ILexer& lexer) : lexer(lexer), aheadBy(0) {}
    
    Token readToken() override {
        if (aheadBy == 0) {
            return lexer.lexToken();
        }
        else {
            return popNext();
        }
    }
    
    Token peekNext() override {
        if (aheadBy >= 1) return next;
        else {
            next = lexer.lexToken();
            aheadBy++;
            return next;
        }
    }
    
    Token peekNextNext() override {
        if (aheadBy >= 2) return nextNext;
        else {
            if (aheadBy == 0) {
                next = lexer.lexToken();
                aheadBy++;
            }
            nextNext = lexer.lexToken();
            aheadBy++;
            return nextNext;
        }
    }
    
    bool isFinished() override {
        return aheadBy == 0 && lexer.isFinished();
    }
};
