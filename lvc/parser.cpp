//
//  parser.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "parser.h"
#include "parserexception.h"
#include <memory>

using namespace ast;

Parser::Parser(IReader &reader, IIssueReporter &issueReporter) :
issueReporter(issueReporter), lexerBuffer(issueReporter, reader) {
    
}

ReportedParserError Parser::reportOnCurrentTok(ParserError er) {
    return issueReporter.report(currentToken.getRow(), currentToken.getStartCol(), er);
}

Module Parser::parse(std::string nameOfModule) {
    
    std::vector<Function> functions;
    
    while (!lexerBuffer.isFinished()) {
        
        // On each iteration, we attempt to parse a function or global (unless empty line).
        try {
            currentToken = lexerBuffer.readToken();
            
            if (currentToken.is(Newline)) continue;
            
            if (lexerBuffer.peekTwoAhead().is(OpenParenthesis)) {
                functions.push_back(parseFunction());
            }
            else {
                
            }
        }
        catch (LexerException &e) {
            assert(false);
        }
        catch (FinishedLexingException &e) {
            assert(false);
        }
        
    }
    
    return Module(nameOfModule, std::move(functions));
    
}

// This function assumes we are at the first token of the function
// (the return type). When it's finished, currentToken should be the
// final dedent token of the function.
Function Parser::parseFunction() {
    issueReporter.log("Parsing function");
    
    FunctionDecl decl(parseFunctionDecl());
    
    currentToken = lexerBuffer.readToken();
    if (currentToken.isNot(Newline)) {
        throw ParserException(reportOnCurrentTok(ParserErrorExpectedNewline));
    }
    currentToken = lexerBuffer.readToken();
    if (currentToken.isNot(Indent)) {
        throw ParserException(reportOnCurrentTok(ParserErrorExpectedIndent));
    }
    
    std::vector<std::unique_ptr<IStmt>> statements;
    while (true) {
        currentToken = lexerBuffer.readToken();
        if (currentToken.is(Newline)) continue;
        if (currentToken.is(Dedent)) break;
        statements.push_back(parseStatement());
    }
    
    return Function(std::move(decl), std::move(statements));
}

// This function assumes currentToken is the first token of the function (the typename).
// After this function is called, currentToken will be the token of the
// closing ) parenthesis for the argument list (which precedes an indent if the function is well-formed and not empty).
FunctionDecl Parser::parseFunctionDecl() {
    issueReporter.log("Parsing function decl");
    
    std::unique_ptr<IType> returnType(parseType());
    
    currentToken = lexerBuffer.readToken();
    if (currentToken.isNot(Identifier)) {
        throw ParserException(reportOnCurrentTok(ParserErrorExpectedIdentifier));
    }
    std::string identifier(currentToken.getStr());
    
    currentToken = lexerBuffer.readToken();
    if (currentToken.isNot(OpenParenthesis)) {
        throw ParserException(reportOnCurrentTok(ParserErrorExpectedParenthesis));
    }
    
    std::vector<ArgumentDecl> arguments;
    #warning TODO: Parse arguments; we'll just leave them empty for now.
    
    currentToken = lexerBuffer.readToken();
    if (currentToken.isNot(CloseParenthesis)) {
        throw ParserException(reportOnCurrentTok(ParserErrorExpectedParenthesis));
    }
    
    return FunctionDecl(std::move(returnType), identifier, std::move(arguments));
}

std::unique_ptr<IType> Parser::parseType() {
    issueReporter.log("Parsing type");
    
    if (currentToken.is(PrimitiveTypenameKind)) {
        return std::make_unique<PrimitiveType>(currentToken.getPrimitiveTypeEnum());
    }
    else if (currentToken.is(Identifier)) {
        #warning TODO: this
        assert(false);
    }
    else {
        throw ParserException(reportOnCurrentTok(ParserErrorExpectedType));
    }
}

// This function assumes currentToken is the first token of the statement.
// When finished, currentToken is the last token of the statement
std::unique_ptr<IStmt> Parser::parseStatement() {
    issueReporter.log("Parsing statement");
    
    #warning TODO: make this recursive when dealing with if statements etc.
    
    if (currentToken.is(KeywordReturn)) {
        currentToken = lexerBuffer.readToken();
        std::unique_ptr<IExp> exp(parseExpression());
        return std::make_unique<ReturnStmt>(std::move(exp));
    }
    
    assert(false);
}

// This function expects currentToken to be the first token of an expression.
// After calling this function, currentToken should be the last token of the expression.
std::unique_ptr<IExp> Parser::parseExpression() {
    return std::make_unique<IntegerLiteralExp>("0");
}

#warning TODO: make static methods so using this is not so stupid
