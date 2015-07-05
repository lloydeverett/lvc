//
//  bufferedlexer.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/05.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <queue>
#include "lexer.h"
#include "iissuereporter.h"
#include "ilexerbuffer.h"

class LexerBuffer : public ILexerBuffer {
private:
    Token next;
    Token nextNext;
    unsigned int aheadBy; // aheadBy can be from 0 (next is not filled) to 2 (nextNext is filled)
    Lexer lexer;
    
    Token popNext() {
        assert(aheadBy > 0);
        Token t = next;
        next = nextNext;
        aheadBy--;
        return t;
    }
public:
    LexerBuffer(IReader& reader, IIssueReporter& issueReporter) : lexer(reader, issueReporter), aheadBy(0) {}
    
    Token readToken() {
        if (aheadBy == 0) {
            return lexer.lexToken();
        }
        else {
            return popNext();
        }
    }
    
    Token peekNext() {
        if (aheadBy >= 1) return next;
        else {
            next = lexer.lexToken();
            aheadBy++;
            return next;
        }
    }
    
    Token peekNextNext() {
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
    
    bool isFinished(IIssueReporter &issueReporter) {
        return aheadBy == 0 && lexer.isFinished();
    }
};
