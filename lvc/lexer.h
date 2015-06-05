//
//  lexer.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/04.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "ilexer.h"
#include "ireader.h"
#include <stack>
#include <queue>
#include "integertypedefs.h"

class QueuedDedent {
public:
    const colnumber startCol;
    const charcount length;
    
    QueuedDedent(colnumber startCol, charcount length) : startCol(startCol), length(length) {}
};

class Lexer : public ILexer {
private:
    IReader &reader;
    bool isNewlyCreatedBool;
    std::stack<colnumber> indentStack;
    std::queue<QueuedDedent> queuedDedents;
    
    bool addDedentsToQueueUntilColnumberIsReached(colnumber col);
    Token makeIndentToken(colnumber col);
    Token popQueuedDedent();
    Token getTokenFromQueuedDedent(QueuedDedent q);
public:
    Lexer(IReader &reader);
    virtual bool isNewlyCreated() override;
    virtual bool isFinished() override;
    virtual Token lexToken(IIssueReporter &issueReporter) override;
    virtual bool didLastLexInvolveSubstitution() override;
    virtual bool attemptToRecoverBySkippingLine() override;
    virtual bool attemptToRecoverBySkippingUntilValidIndentation() override;
};
