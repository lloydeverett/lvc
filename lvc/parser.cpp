//
//  parser.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "parser.h"

Parser::Parser(IReader &reader, IIssueReporter &issueReporter) :
issueReporter(issueReporter), reader(reader), lexer(reader) {
    
}

Token Parser::peekAhead(unsigned int n) {
    while (n < tokenQueue.size()) {
        tokenQueue.emplace(lexer.lexToken(issueReporter));
    }
    return tokenQueue.back();
}

// The contents of currentToken should ONLY be changed
// through this method
void Parser::readNextToken() {
    if (tokenQueue.size() > 0) {
        currentToken = tokenQueue.front();
        tokenQueue.pop();
        return;
    }
    
    currentToken = lexer.lexToken(issueReporter);
}

Module Parser::parseModule() {
    // This function parses at the module,
    // and defers the work below this level to other functions.
    // Thus we pretty much just need to identify global variables
    // and functions, and hand them off to lower functions.
    
    try {
        readNextToken();
        if (currentToken.isTypenameKeyword()) {
            
        }
    }
    
}

Function Parser::parseFunction() {
    
}

std::unique_ptr<IStmt> Parser::parseStatement() {
    
}

#warning TODO: make static methods so using this is not so stupid
