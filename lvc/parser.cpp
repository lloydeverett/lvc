//
//  parser.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "parser.h"

Parser::Parser(IReader &reader, IIssueReporter &issueReporter) :
issueReporter(issueReporter), lexerBuffer(issueReporter, reader) {
    
}

Module Parser::parseModule() {
    // This function parses at the module level
    // and defers the work below this level to other functions.
    // Thus we pretty much just need to identify global variables
    // and functions, and hand them off to lower functions.
    
    std::vector<Function> functions;
    
    while (!lexerBuffer.isFinished()) {
        try {
            currentToken = lexerBuffer.readToken();
            
            if (currentToken.isTypenameKeyword()) {
                if (lexerBuffer.peekAhead(2).is(OpenParenthesis)) {
                    functions.push_back(parseFunction());
                    continue;
                }
                else {
                    
                }
            }
        }
    }
    
    
    try {
        currentToken = lexerBuffer.readToken();
        
        if (currentToken.isTypenameKeyword()) {
            if (lexerBuffer.peekAhead(2).is(OpenParenthesis)) {
                parseFunction();
            }
        }
    }
    catch (LexerException &e) {
    }
    catch (FinishedLexingException &e) {
        
    }
}



Function Parser::parseFunction() {
    
}

std::unique_ptr<IStmt> Parser::parseStatement() {
    
}

#warning TODO: make static methods so using this is not so stupid
