//
//  parser.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "parser.h"
#include "parserexception.h"

Parser::Parser(IReader &reader, IIssueReporter &issueReporter) :
issueReporter(issueReporter), lexerBuffer(issueReporter, reader) {
    
}

ReportedParserError Parser::reportOnCurrentTok(ParserError er) {
    return issueReporter.report(currentToken.getRow(), currentToken.getStartCol(), er);
}

Module Parser::parseModule() {
    // This function parses at the module level
    // and defers the work below this level to other functions.
    // Thus we pretty much just need to identify global variables
    // and functions, and hand them off to lower functions.
    
    std::vector<Function> functions;
    
    while (!lexerBuffer.isFinished()) {
        
        // On each iteration, we attempt to parse a function or global.
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
            
        }
        catch (FinishedLexingException &e) {
            
        }
        
    }
    
    return Module(functions);
}

// This function assumes we are at the first token of the function
// (the return type). When it's finished, currentToken should be at the
// final dedent token of the function.
Function Parser::parseFunction() {
    issueReporter.log("Parsing function");
    
    FunctionDecl decl(parseFunctionDecl());
    
    currentToken = lexerBuffer.readToken();
    if (currentToken.isNot(Indent)) {
        throw ParserException(issueReporter.report(currentToken.getRow(), currentToken.getStartCol(), ParserErrorExpectedIndent));
    }
    
    while (currentToken.isNot(Dedent)) {
        parseStatement();
    }
}

// This function assumes we are at the first token of the function (the typename).
// After this function is called, currentToken will be the token of the
// closing ) parenthesis for the argument list (the next token should be an indent).
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
        return std::make_unique<PrimitiveType>(currentToken.getPrimitiveTypename());
    }
    else if (currentToken.is(Identifier)) {
        #warning TODO: this
        assert(false);
    }
    else {
        throw ParserException(reportOnCurrentTok(ParserErrorExpectedType));
    }
}

std::unique_ptr<IStmt> Parser::parseStatement() {
    issueReporter.log("Parsing statement");
    
    
}

std::unique_ptr<IExp> Parser::parseExpression() {
    
}

#warning TODO: make static methods so using this is not so stupid
