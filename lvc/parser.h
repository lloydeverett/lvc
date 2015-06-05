//
//  parser.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "ast.h"
#include "ireader.h"
#include "iissuereporter.h"
#include "ilexer.h"
#include "lexer.h"

class Parser {
private:
    Token currentToken;
    IIssueReporter &issueReporter;
    Lexer lexer;
    
public:
    Parser(IIssueReporter &issueReporter);
    Module parseModule();
    Function parseFunction();
    std::unique_ptr<IStmt> parseStatement();
};
