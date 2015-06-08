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

ReportedParserError Parser::reportOnCurrentTok(ParserError er) {
    return issueReporter.report(currentToken.getRow(), currentToken.getStartCol(), er);
}

Function Parser::parseFunction() {
    issueReporter.log("Parsing function");
    
    // Parse FunctionDecl.
    // We assume currentToken is the first token of the
    // function (the return type).
    
    std::unique_ptr<IType> returnType;
    if (currentToken.is(PrimitiveTypenameKind)) {
        returnType = std::make_unique<PrimitiveType>(currentToken.getPrimitiveTypename());
    }
    else {
        assert(false);
    }
    
    currentToken = lexerBuffer.readToken();
    if (currentToken.isNot(Identifier)) {
        throw ParserException(reportOnCurrentTok(ParserErrorExpectedIdentifier));
    }
    std::string identifier(currentToken.getStr());
    
    currentToken = lexerBuffer.readToken();
    if (currentToken.isNot(OpenParenthesis)) {
        throw ParserException(reportOnCurrentTok(ParserErrorExpectedParenthesis));
    }
    
    std::vector<A
    
    //FunctionDecl(<#std::unique_ptr<IType> returnType#>, <#std::string identifier#>, <#std::vector<std::unique_ptr<ArgumentDecl> > arguments#>)
    currentToken = lexerBuffer.readToken();
    if(currentToken.isNot(Identifier)) {
        throw ParserException(issueReporter.report(currentToken.getRow(), currentToken.getStartCol(), ParserErrorExpectedIdentifier));
    }
    std::string identifier = currentToken.getStr();
    
    currentToken = lexerBuffer.readToken();
    assert(currentToken.is(OpenParenthesis));
    
    std::vector<std::unique_ptr<ArgumentDecl>> arguments;
    // Parse argument list
#warning TODO: parse argument list (right now we just expect ')')
    //do {
      //  currentToken = lexerBuffer.readToken();
     //
   // } while (currentToken.isNot(CloseParenthesis));
    currentToken =  lexerBuffer.readToken();
    assert(currentToken.is(CloseParenthesis));
    
    FunctionDecl decl(
    
    currentToken = lexerBuffer.readToken();
    if (currentToken.isNot(Indent)) {
        throw ParserException(issueReporter.report(currentToken.getRow(), currentToken.getStartCol(), ParserErrorExpectedIndent));
    }
    
    while (currentToken.isNot(Dedent)) {
        parseStatement();
    }
}

std::unique_ptr<IStmt> Parser::parseStatement() {
    issueReporter.log("Parsing statement");
}

#warning TODO: make static methods so using this is not so stupid
