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

void Parser::readNextToken() {
    
}

Module Parser::parseModule() {
    // This function parses at the module,
    // and defers the work below this level to other functions.
    // Thus we pretty much just need to identify global variables
    // and functions, and hand them off to lower functions.
    
    
}

Function Parser::parseFunction() {
    
}

std::unique_ptr<IStmt> Parser::parseStatement() {
    
}

Module Parser::parseModule(ILexer &lexer, IIssueReporter &issueReporter) {
    return Module(std::vector<Function>());
}

#warning TODO: make static methods so using this is not so stupid
