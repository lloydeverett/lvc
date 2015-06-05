//
//  parser.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "ast.h"
#include "stringreader.h"
#include "iissuereporter.h"
#include "lexer.h"

class Parser {
private:
    Token currentToken;
    void readNextToken();
    IIssueReporter &issueReporter;
    IReader &reader;
    Lexer lexer;
    
public:
    Parser(IReader &reader, IIssueReporter &issueReporter);
    Module parseModule();
    Function parseFunction();
    std::unique_ptr<IStmt> parseStatement();
};
