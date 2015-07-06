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
#include "ilexerbuffer.h"
#include "token.h"
#include "parserexceptions.h"

class Parser {
private:
    IIssueReporter& issueReporter;
    ILexerBuffer& lexerBuffer;
    Token currentToken;
    
    void readTokenIntoCurrent();
    void reportOnCurrentToken(const std::string& message);
public:
    Parser(ILexerBuffer &lexerBuffer, IIssueReporter &issueReporter);
    ast::FunctionDecl parseFunctionDecl();
    ast::Function parseFunction();
    ast::BlockStmt parseBlock();
    ast::VariableDeclStmt parseVariableDeclStmt(std::unique_ptr<ast::IType> type);
    std::string parseIdentifier();
    std::unique_ptr<ast::IStmt> parseStatement();
    std::unique_ptr<ast::IExp> parseParenExpression();
    std::unique_ptr<ast::IExp> parseExpression();
    std::unique_ptr<ast::IExp> parseBinopRhs(std::unique_ptr<ast::IExp> lhs, BinopCode afterLhsCode, int minPrecedence);
    std::unique_ptr<ast::IExp> parsePrimaryExpression();
    boost::optional<std::unique_ptr<ast::IType>> tryParseTypename();
    std::unique_ptr<ast::IType> parseTypename();
    boost::optional<BinopCode> tryParseBinopCode();
    ast::Module parseModule(std::string name);
    static int getBinopCodePrecedence(BinopCode code);
};
