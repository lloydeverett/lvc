//
//  lexer.h
//  lvc
//
//  Created by Lloyd Everett on 2015/05/23.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <stack>
#include <queue>
#include "integertypedefs.h"
#include "iissuereporter.h"
#include "token.h"
#include "ireader.h"

class QueuedDedent {
    colnumber startCol;
    charcount length;
    
public:
    QueuedDedent(colnumber startCol, charcount length) : startCol(startCol), length(length) {}
    
    colnumber getStartCol() { return startCol; }
    charcount getLength() { return length; }
};

class Lexer {
private:
    std::stack<colnumber> indentationStack;
    std::queue<QueuedDedent> queuedDedents;
    IIssueReporter &issueReporter;
    IReader &reader;
    void popQueuedDedent(Token &tok);
    bool addDedentsToQueueUntilColnumberIsReached(colnumber col);
    void makeIndentToken(Token &tok, colnumber col);
    
public:
    Lexer(IReader &reader, IIssueReporter &issueReporter);
    bool readNextToken(Token &tok);
    bool isFinished();
    
};
