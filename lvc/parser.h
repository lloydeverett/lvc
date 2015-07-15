//
//  parser.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <map>
#include <boost/optional.hpp>
#include "ast.h"
#include "ireader.h"
#include "iissuereporter.h"
#include "itokeninputstream.h"
#include "token.h"
#include "parserexceptions.h"

class Parser {
private:
    IIssueReporter& issueReporter;
    ITokenInputStream& inputStream;
    Token currentToken;
    
    void readTokenIntoCurrent();
    void reportOnCurrentToken(const std::string& message);
    std::unique_ptr<ast::IExp> parseBinopRhs(std::unique_ptr<ast::IExp> lhs, BinopCode afterLhsCode, int minPrecedence);
    std::unique_ptr<ast::IExp> parsePrimaryExpression();
public:
    Parser(ITokenInputStream& inputStream, IIssueReporter& issueReporter);
    ast::FunctionDecl parseFunctionDecl();
    ast::Function parseFunction();
    ast::BlockStmt parseBlock();
    ast::VariableDeclStmt parseVariableDeclStmt(std::unique_ptr<ast::IType> type);
    std::string parseIdentifier();
    std::unique_ptr<ast::IStmt> parseStatement();
    std::unique_ptr<ast::IExp> parseParenExpression();
    std::unique_ptr<ast::IExp> parseExpression();
    boost::optional<std::unique_ptr<ast::IType>> tryParseType();
    std::unique_ptr<ast::IType> parseType();
    boost::optional<BinopCode> tryParseBinopCode();
    ast::Module parseModule(std::string name);
    static int getBinopCodePrecedence(BinopCode code);
};
