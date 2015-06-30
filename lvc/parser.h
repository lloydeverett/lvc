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
#include "parserexceptions.h"
#include "locals.h"

class Parser {
private:
    IIssueReporter &issueReporter;
    LexerBuffer lexerBuffer;
    Token currentToken;
    
    Token& readTokenIntoCurrent();
    ast::FunctionDecl parseFunctionDecl();
    void reportOnCurrentToken(const std::string& message);
    ast::Function parseFunction();
    ast::BlockStmt parseBlock();
    std::unique_ptr<ast::IStmt> parseStatement();
    ast::VariableDeclStmt parseVariableDeclStmt(std::unique_ptr<ast::IType> type);
    std::unique_ptr<ast::IExp> parseParenExpression();
    std::unique_ptr<ast::IExp> parseExpression();
    std::unique_ptr<ast::IExp> parseBinopRhs(std::unique_ptr<ast::IExp> lhs, BinopCode afterLhsCode, int minPrecedence);
    std::unique_ptr<ast::IExp> parsePrimaryExpression();
    std::string parseIdentifier();
    boost::optional<std::unique_ptr<ast::IType>> tryParseTypename();
    std::unique_ptr<ast::IType> parseTypename();
    boost::optional<BinopCode> tryParseBinopCode();
    int getBinopCodePrecedence(BinopCode code);
public:
    Parser(IReader &reader, IIssueReporter &issueReporter);
    ast::Module parse(std::string nameOfModule);
};
