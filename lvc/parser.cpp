//
//  parser.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "parser.h"
#include <memory>

#warning TODO: make using this class more convenient

using namespace ast;

Parser::Parser(IReader &reader, IIssueReporter &issueReporter) :
issueReporter(issueReporter), lexerBuffer(issueReporter, reader) {
    
}

Token& Parser::readTokenIntoCurrent() {
    currentToken = lexerBuffer.readToken();
    return currentToken;
}

void Parser::reportOnCurrentToken(const std::string& message) {
    issueReporter.report(currentToken.getRow(), currentToken.getStartCol(), message, SubsystemParser);
}

Module Parser::parse(std::string nameOfModule) {
    
    std::vector<Function> functions;
    
    while (true) {
        try {
            if (currentToken.is(Eof)) break;
            
            if (currentToken.is(Newline)) {
                readTokenIntoCurrent();
                continue;
            }
            
            if (lexerBuffer.peekNextNext().is(OpenParenthesis)) {
                functions.push_back(parseFunction());
            }
            else {
                #warning TOOD: parse global
            }
        }
        catch (LexerException &e) {
            #warning TODO: implement this
            assert(false);
        }
        catch (LexerFinishedException &e) {
            assert(false);
        }
    }
    
    return Module(nameOfModule, std::move(functions));
    
}

Function Parser::parseFunction() {
    FunctionDecl decl(parseFunctionDecl());
    
    #warning TOOD: handle same-line functions.
    if (currentToken.isNot(Newline)) {
        reportOnCurrentToken("Expected a newline after function declaration.");
        throw ParserErrorException(ParserErrorExpectedNewline);
    }
    readTokenIntoCurrent();
    if (currentToken.isNot(Indent)) {
        reportOnCurrentToken("Expected indent on new line after function declaration.");
        throw ParserErrorException(ParserErrorExpectedIndent);
    }
    readTokenIntoCurrent();
    
    symbols.openScope();
    
    for (ArgumentDecl &argDecl : decl.arguments) {
        symbols.create(&argDecl);
    }
    
    std::vector<std::unique_ptr<IStmt>> statements;
    while (currentToken.isNot(Dedent)) {
        if (currentToken.is(Newline)) {
            // empty line
            readTokenIntoCurrent();
            continue;
        }
        statements.push_back(parseStatement());
    }
    
    symbols.closeScope();
    
    return Function(std::move(decl), std::move(statements));
}

FunctionDecl Parser::parseFunctionDecl() {
    std::unique_ptr<IType> returnType(parseType());
    std::string identifier(parseIdentifier());
    
    if (currentToken.isNot(OpenParenthesis)) {
        reportOnCurrentToken("Expected '(' after identifier in function declaration.");
        throw ParserErrorException(ParserErrorExpectedOpenParenthesis);
    }
    readTokenIntoCurrent();
    
    std::vector<ArgumentDecl> arguments;
    if (currentToken.isNot(CloseParenthesis)) {
        while (true) {
            std::unique_ptr<IType> argType(parseType());
            std::string argIdentifier(parseIdentifier());
            
            boost::optional<std::unique_ptr<IExp>> optDefaultValue = boost::none;
            if (currentToken.is(Equals)) {
                readTokenIntoCurrent();
                optDefaultValue = parseExpression();
            }
            
            VariableDecl varDecl = VariableDecl(std::move(argType), argIdentifier);
            arguments.emplace_back(std::move(varDecl), optDefaultValue);
            
            if (currentToken.is(CloseParenthesis)) break;
            if (currentToken.is(Comma)) {
                readTokenIntoCurrent();
                continue;
            }
            
            reportOnCurrentToken("Badly formed function declaration. Expected comma or ')'.");
            throw ParserErrorException(ParserErrorExpectedCommaOrCloseParenthesis);
        }
    }
    readTokenIntoCurrent(); // read token after the close parenthesis into currentToken
    
    return FunctionDecl(std::move(returnType), identifier, std::move(arguments));
}

boost::optional<std::unique_ptr<IType>> Parser::tryParseType() {
    PrimitiveTypeCode code;
    switch (currentToken.getKind()) {
        case Char: code = PrimitiveTypeCodeChar; break;
        case Short: code = PrimitiveTypeCodeShort; break;
        case Int: code = PrimitiveTypeCodeInt; break;
        case Long: code = PrimitiveTypeCodeLong; break;
        case Uchar: code = PrimitiveTypeCodeUchar; break;
        case Ushort: code = PrimitiveTypeCodeUshort; break;
        case Uint: code = PrimitiveTypeCodeUint; break;
        case Ulong: code = PrimitiveTypeCodeUlong; break;
        case Bool: code = PrimitiveTypeCodeBool; break;
        case Float: code = PrimitiveTypeCodeFloat; break;
        case Double: code = PrimitiveTypeCodeDouble; break;
        default:
            return boost::none;
    }
    std::unique_ptr<IType> ret = std::make_unique<PrimitiveType>(code);
    readTokenIntoCurrent();
    return std::move(ret);
}

std::unique_ptr<IType> Parser::parseType() {
    boost::optional<std::unique_ptr<IType>> opt = tryParseType();
    if (!opt) {
        reportOnCurrentToken("Expected typename.");
        throw ParserErrorException(ParserErrorExpectedType);
    }
    return std::move(*opt);
}

// After this function is called, currentToken will be the token after the newline that terminates the statement.
std::unique_ptr<IStmt> Parser::parseStatement() {
    
    if (currentToken.is(If)) {
        readTokenIntoCurrent();
        std::unique_ptr<IExp> condition = parseParenExpression();
        
#warning TODO: make parse if
    }
    
    if (currentToken.is(Return)) {
        readTokenIntoCurrent();
        std::unique_ptr<IExp> exp(parseExpression());
        return std::make_unique<ReturnStmt>(std::move(exp));
    }

    
    boost::optional<std::unique_ptr<IType>> optType = tryParseType();
    if (optType) {
        
    }
    
    assert(false);
}

std::unique_ptr<IExp> Parser::parseParenExpression() {
    if (currentToken.isNot(OpenParenthesis)) {
        reportOnCurrentToken("Expected '('");
        throw ParserErrorException(ParserErrorExpectedOpenParenthesis);
    }
    readTokenIntoCurrent();
    
    std::unique_ptr<IExp> exp(parseExpression());
    
    if (currentToken.isNot(CloseParenthesis)) {
        reportOnCurrentToken("Expected ')'");
        throw ParserErrorException(ParserErrorExpectedCloseParenthesis);
    }
    readTokenIntoCurrent();
    
    return std::move(exp);
}

std::unique_ptr<IExp> Parser::parseExpression() {
    std::unique_ptr<IExp> lhs = parsePrimaryExpression();
    
}

std::unique_ptr<IExp> Parser::parseRHSExpression(int precedence) {
    
}

// A primary expression is an expression that is not a BinOpExp.
// This dinstinction is important becausee BinOpExps require operator precedence parsing.
// Note that parseExpression can still parse primary expressions; this function
// is called by it.
std::unique_ptr<IExp> Parser::parsePrimaryExpression() {
    if (currentToken.is(Identifier)) {
        if (lexerBuffer.peekNext().is(OpenParenthesis)) {\
            // Function call
            readTokenIntoCurrent(); // read open parenthesis -> currentToken
            readTokenIntoCurrent(); // read token after it -> currentToken
            std::vector<ArgumentDecl> args;
            if (currentToken.isNot(CloseParenthesis)) {
                while (true) {
                    ArgumentDecl def;
                    parseExpression();
#warning TODO: lookup symbol for function
                    
                    if (currentToken.is(CloseParenthesis)) break;
                    if (currentToken.is(Comma)) {
                        readTokenIntoCurrent();
                        continue;
                    }
                    
                    reportOnCurrentToken("Expected comma or ')' in argument list.");
                    throw ParserErrorException(ParserErrorExpectedCommaOrCloseParenthesis);
                }
            }
            readTokenIntoCurrent(); // read token after close parenthesis -> currentToken
            
        }
        else {
            // Variable Expression
        }
    }
    if (currentToken.is(Identifier)) {
        if (!lexerBuffer.isFinished(issueReporter) && lexerBuffer.peekNext().is(OpenParenthesis)) {
            // Function call
            readTokenIntoCurrent(); // read the open parenthesis
            readTokenIntoCurrent(); // read the token after the open parenthesis
            if (currentToken.isNot(CloseParenthesis)) {
                parseExpression(7);
            }
        }
        else {
            // Variable
            
        }
    }
    else if (currentToken.is(IntegerLiteral)) {
        return std::make_unique<IntegerLiteralExp>(currentToken.getStr());
    }
    else if (currentToken.is(RealLiteral)) {
        return std::make_unique<RealLiteralExp>(currentToken.getStr());
    }
    assert(false);
}

std::string Parser::parseIdentifier() {
    if (currentToken.isNot(Identifier)) {
        reportOnCurrentToken("Expected identifier.");
        throw ParserErrorException(ParserErrorExpectedIdentifier);
    }
    std::string s = currentToken.getStr();
    readTokenIntoCurrent();
    return s;
}

boost::optional<BinopCode> Parser::tryParseBinopCode() {
    BinopCode code;
    switch (currentToken.getKind()) {
        case Plus: code = BinopCodeAdd; break;
        case Minus: code = BinopCodeSubtract; break;
        case Asterisk: code = BinopCodeMultiply; break;
        case Slash: code = BinopCodeDivide; break;
        default:
            return boost::none;
    }
    readTokenIntoCurrent();
    return code;
}
