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

class ParserErrorException : public ParserException {
protected:
    ParserErrorException() {}
};

class ParserErrorExceptionExpectedNewline : public ParserErrorException {};
class ParserErrorExceptionExpectedIndent : public ParserErrorException {};
class ParserErrorExceptionExpectedIdentifier : public ParserErrorException {};
class ParserErrorExceptionExpectedOpenParenthesis : public ParserErrorException {};
class ParserErrorExceptionExpectedCloseParenthesis : public ParserErrorException {};
class ParserErrorExceptionExpectedType : public ParserErrorException {};
class ParserErrorExceptionExpectedCommaOrCloseParenthesis : public ParserErrorException {};
class ParserErrorExceptionUnknownStatementBeginning : public ParserErrorException {};
class ParserErrorExceptionUnknownLineBeginningAtModuleLevel : public ParserErrorException {};
class ParserErrorExceptionUnknownExpressionBeginning : public ParserErrorException {};
