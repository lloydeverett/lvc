//
//  parser.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <map>
#include "ast.h"
#include "ireader.h"
#include "iissuereporter.h"
#include "lexerbuffer.h"

class Parser {
private:
    IIssueReporter &issueReporter;
    LexerBuffer lexerBuffer;
    Token currentToken;
    
    std::map<std::string, IDecl&> moduleLevelSymbols;
    std::map<std::string, IDecl&> functionLevelSymbols;
    FunctionDecl parseFunctionDecl();
    ReportedParserError reportOnCurrentTok(ParserError er);
    Function parseFunction();
    std::unique_ptr<IStmt> parseStatement();
    std::unique_ptr<IExp> parseExpression();
    std::unique_ptr<IType> parseType();
public:
    Parser(IReader &reader, IIssueReporter &issueReporter);
    Module parse();
};
