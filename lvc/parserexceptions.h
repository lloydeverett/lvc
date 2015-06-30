//
//  parserexception.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/06.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <stdexcept>

class ParserException : public std::exception {
protected:
    ParserException() {}
};

enum ParserError {
    ParserErrorExpectedNewline,
    ParserErrorExpectedIndent,
    ParserErrorExpectedIdentifier,
    ParserErrorExpectedOpenParenthesis,
    ParserErrorExpectedCloseParenthesis,
    ParserErrorExpectedType,
    ParserErrorExpectedCommaOrCloseParenthesis,
    ParserErrorUnknownStatementBeginning,
    ParserErrorUnknownLineBeginningAtModuleLevel,
};

#warning TODO: REFACTOR.

class ParserErrorException : public ParserException {
private:
    ParserError error;
public:
    ParserErrorException(ParserError error) : error(error) {}
    ParserError getError() {
        return error;
    }
};
