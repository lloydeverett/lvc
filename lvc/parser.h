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
    
    std::map<std::string, ast::IDecl&> moduleLevelSymbols;
    std::map<std::string, ast::IDecl&> functionLevelSymbols;
    ast::FunctionDecl parseFunctionDecl();
    ReportedParserError reportOnCurrentTok(ParserError er);
    ast::Function parseFunction();
    std::unique_ptr<ast::IStmt> parseStatement();
    std::unique_ptr<ast::IExp> parseExpression();
    std::unique_ptr<ast::IType> parseType();
public:
    Parser(IReader &reader, IIssueReporter &issueReporter);
    ast::Module parse(std::string nameOfModule);
};
