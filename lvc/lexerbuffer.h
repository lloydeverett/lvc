//
//  bufferedlexer.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/05.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <queue>
#include "ilexer.h"
#include "iissuereporter.h"

class LexerBuffer {
private:
    std::queue<Token> queue;
    ILexer &lexer;
    IIssueReporter &issueReporter;
    
public:
    LexerBuffer(IIssueReporter &issueReporter, ILexer &lexer) : issueReporter(issueReporter), lexer(lexer) {
        
    }
    
    Token nextToken() {
        if (queue.size() > 0) {
            Token t = queue.front();
            queue.pop();
            return t;
        }
        
        return lexer.lexToken(issueReporter);
    }
    
    Token peekAhead(unsigned int n) {
        while (n < queue.size()) {
            queue.push(lexer.lexToken(issueReporter));
        }
        
        return queue.back();
    }
};
