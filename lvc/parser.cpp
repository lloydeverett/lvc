//
//  parser.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "parser.h"
#include <memory>

using namespace ast;

Parser::Parser(IReader &reader, IIssueReporter &issueReporter) :
issueReporter(issueReporter), lexerBuffer(issueReporter, reader) {
    
}

void Parser::reportOnCurrentToken(const std::string& message) {
    issueReporter.report(currentToken.getRow(), currentToken.getStartCol(), message, SubsystemParser);
}

Module Parser::parse(std::string nameOfModule) {
    
    std::vector<Function> functions;
    
    while (!lexerBuffer.isFinished(issueReporter)) {
        
        // On each iteration, we attempt to parse a function or global (unless empty line).
        try {
            currentToken = lexerBuffer.readToken();
            
            if (currentToken.is(Newline)) continue;
            
            if (lexerBuffer.peekNextNext().is(OpenParenthesis)) {
                functions.push_back(parseFunction());
            }
            else {
                
            }
        }
        catch (LexerException &e) {
            assert(false);
        }
        catch (LexerFinishedException &e) {
            assert(false);
        }
        
    }
    
    return Module(nameOfModule, std::move(functions));
    
}

// This function assumes we are at the first token of the function
// (the return type). When it's finished, currentToken should be the
// final dedent token of the function.
Function Parser::parseFunction() {
    FunctionDecl decl(parseFunctionDecl());
    
    currentToken = lexerBuffer.readToken();
    #warning TOOD: handle same-line functions.
    if (currentToken.isNot(Newline)) {
        reportOnCurrentToken("Expected a newline after function declaration.");
        throw ParserErrorException(ParserErrorExpectedNewline);
    }
    currentToken = lexerBuffer.readToken();
    if (currentToken.isNot(Indent)) {
        reportOnCurrentToken("Expected indent on new line after function declaration.");
        throw ParserErrorException(ParserErrorExpectedIndent);
    }
    
    symbols.openScope();
    
    std::vector<std::unique_ptr<IStmt>> statements;
    while (true) {
        currentToken = lexerBuffer.readToken();
        if (currentToken.is(Newline)) continue; // skip empty lines
        if (currentToken.is(Dedent)) break;
        statements.push_back(parseStatement());
    }
    
    symbols.closeScope();
    
    return Function(std::move(decl), std::move(statements));
}

// This function assumes currentToken is the first token of the function (the typename).
// After this function is called, currentToken will be the token of the
// closing ) parenthesis for the argument list (which precedes an indent if the function is well-formed and not empty).
FunctionDecl Parser::parseFunctionDecl() {
    std::unique_ptr<IType> returnType(parseType());
    
    currentToken = lexerBuffer.readToken();
    if (currentToken.isNot(Identifier)) {
        reportOnCurrentToken("Expected identifier at beginning of function declaration.");
        throw ParserErrorException(ParserErrorExpectedIdentifier);
    }
    std::string identifier = currentToken.getStr();
    
#warning TODO: symbol
    
    currentToken = lexerBuffer.readToken();
    if (currentToken.isNot(OpenParenthesis)) {
        reportOnCurrentToken("Expected open parenthesis after identifier in function declaration.");
        throw ParserErrorException(ParserErrorExpectedParenthesis);
    }
    
    std::vector<ArgumentDecl> arguments;
    #warning TODO: Parse arguments; we'll just leave them empty for now.
    
    currentToken = lexerBuffer.readToken();
    if (currentToken.isNot(CloseParenthesis)) {
        reportOnCurrentToken("Expected close parenthesis");
        throw ParserErrorException(ParserErrorExpectedCloseParenthesis);
    }
    
    return FunctionDecl(std::move(returnType), identifier, std::move(arguments));
}

#warning todo: make tryParseType

boost::optional<std::unique_ptr<ast::IType>> Parser::tryParseType() {
    unsigned int code;
    switch (currentToken.getKind()) {
        case Char: code = ast::PrimitiveType::CHAR_CODE; break;
        case Short: code = ast::PrimitiveType::SHORT_CODE; break;
        case Int: code = ast::PrimitiveType::INT_CODE; break;
        case Long: code = ast::PrimitiveType::LONG_CODE; break;
        case Uchar: code = ast::PrimitiveType::UCHAR_CODE; break;
        case Ushort: code = ast::PrimitiveType::USHORT_CODE; break;
        case Uint: code = ast::PrimitiveType::UINT_CODE; break;
        case Ulong: code = ast::PrimitiveType::ULONG_CODE; break;
        case Bool: code = ast::PrimitiveType::BOOL_CODE; break;
        case Float: code = ast::PrimitiveType::FLOAT_CODE; break;
        case Double: code = ast::PrimitiveType::DOUBLE_CODE; break;
        default:
            return boost::none;
    }
    std::unique_ptr<ast::IType> ret = std::make_unique<ast::PrimitiveType>(code);
    return std::move(ret);
}

std::unique_ptr<ast::IType> Parser::parseType() {
    boost::optional<std::unique_ptr<ast::IType>> opt = tryParseType();
    if (!opt) {
        reportOnCurrentToken("Expected type specifier.");
        throw ParserErrorException(ParserErrorExpectedType);
    }
    return std::move(*opt);
}

// This function assumes currentToken is the first token of the statement.
// When finished, currentToken is the last token of the statement (including the newline)
std::unique_ptr<IStmt> Parser::parseStatement() {
#warning todo istype
    
    if (currentToken.is(If)) {
        lexerBuffer.readToken();
        parseStatement();
#warning TODO: THI
    }
    
    if (currentToken.is(Return)) {
        currentToken = lexerBuffer.readToken();
        std::unique_ptr<IExp> exp(parseExpression());
        return std::make_unique<ReturnStmt>(std::move(exp));
    }

    
    boost::optional<std::unique_ptr<IType>> optType = tryParseType();
    if (optType) {
        
    }
    
    assert(false);
}

// This function expects currentToken to be the first token of an expression.
// After calling this function, currentToken should be the last token of the expression.
std::unique_ptr<IExp> Parser::parseExpression() {
    
    return std::make_unique<IntegerLiteralExp>("0");
}

// A primary expression is an expression that is not a BinOpExp.
// This dinstinction is important becausee BinOpExps require operator precedence parsing.
// Note that parseExpression can still parse primary expressions; this function
// is called by it.
std::unique_ptr<ast::IExp> Parser::parsePrimaryExpression() {
    if (currentToken.is(Identifier)) {
        if (!lexerBuffer.isFinished(issueReporter) && lexerBuffer.peekNext().is(OpenParenthesis)) {
            // Function call
            lexerBuffer.readToken(); // read the open parenthesis
            lexerBuffer.readToken(); // read the token after the open parenthesis
#warning TOOD: rmemeber tpo set currentTOken
            while (currentToken.isNot(CloseParenthesis)) {
                parseExpression();
                currentToken = lexerBuffer.readToken();
                std::string identifier = currentToken.getStr();
            }
        }
        else {
            // Variable
            
        }
    }
    assert(false);
}

#warning TODO: make static methods so using this is not so stupid
