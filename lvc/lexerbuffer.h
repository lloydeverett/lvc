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

class LexerBuffer {
private:
    Token next;
    Token nextNext;
    unsigned int aheadBy; // aheadBy can be from 0 (next is not filled) to 2 (nextNext is filled)
    Lexer lexer;
    IIssueReporter &issueReporter;
    
    Token popNext() {
        assert(aheadBy > 0);
        Token t = next;
        next = nextNext;
        aheadBy--;
        return t;
    }
public:
    LexerBuffer(IIssueReporter &issueReporter, IReader &reader) : issueReporter(issueReporter), lexer(reader) {
        aheadBy = 0;
    }
    
    Token readToken() {
        if (aheadBy == 0) {
            return lexer.lexToken(issueReporter);
        }
        else {
            return popNext();
        }
    }
    
    Token peekOneAhead() {
        if (aheadBy >= 1) return next;
        else {
            next = lexer.lexToken(issueReporter);
            aheadBy++;
            return next;
        }
    }
    
    Token peekTwoAhead() {
        if (aheadBy >= 2) return nextNext;
        else {
            if (aheadBy == 0) {
                next = lexer.lexToken(issueReporter);
                aheadBy++;
            }
            nextNext = lexer.lexToken(issueReporter);
            aheadBy++;
            return nextNext;
        }
    }
    
    bool isFinished() {
        return aheadBy == 0 && lexer.isFinished();
    }
};
