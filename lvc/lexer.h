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

class Lexer : public ILexer {
private:
    class QueuedDedent {
    public:
        const colnumber startCol;
        const charcount length;
        
        QueuedDedent(colnumber startCol, charcount length) : startCol(startCol), length(length) {}
    };
    
    IIssueReporter& issueReporter;
    IReader& reader;
    std::stack<colnumber> indentStack;
    std::queue<QueuedDedent> queuedDedents;
    
    bool addDedentsToQueueUntilColnumberIsReached(colnumber col);
    Token makeIndentToken(colnumber col);
    Token getTokenFromQueuedDedent(QueuedDedent q);
    bool tryToSkipComment();
    void skipCommentsAndNonIndentWhitespace();
    bool hasProducedEof;
public:
    Lexer(IReader& reader, IIssueReporter& issueReporter);
    virtual bool isFinished() override;
    virtual Token lexToken() override;
};
