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
#include "lexerbuffer.h"

class Parser {
private:
    IIssueReporter &issueReporter;
    LexerBuffer lexerBuffer;
    Token currentToken;
    
public:
    Parser(IReader &reader, IIssueReporter &issueReporter);
    Module parseModule();
    Function parseFunction();
    std::unique_ptr<IStmt> parseStatement();
    std::unique_ptr<IExp> parseExpression();
};
