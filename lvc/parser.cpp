//
//  parser.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "parser.h"

Parser::Parser(IIssueReporter &issueReporter) : issueReporter(issueReporter), lexer() {
    
}

Module parseModule() {
    
}

Function parseFunction() {
    
}

std::unique_ptr<IStmt> parseStatement() {
    
}

Module parseModule(ILexer &lexer, IIssueReporter &issueReporter) {
    return Module(std::vector<Function>());
}
