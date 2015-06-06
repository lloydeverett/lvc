//
//  iissuereporter.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "integertypedefs.h"
#include <string>
#include <cassert>

enum LexerError {
    LexerErrorInvalidDedent,
    LexerErrorInvalidNumberLiteral,
    LexerErrorUnterminatedBlockComment,
    LexerErrorInvalidNewline,
};

inline const char* getMessageForLexerError(LexerError lexerError) {
    return lexerError == LexerErrorInvalidDedent             ? "Invalid indentation (could not parse valid dedent token)." :
           lexerError == LexerErrorInvalidNumberLiteral      ? "Could not lex number literal." :
           lexerError == LexerErrorUnterminatedBlockComment  ? "Block comment was not terminated." :
           lexerError == LexerErrorInvalidNewline            ? "Invalid newline. Newlines should be CRLF or LF." :
           (assert(false), "");
}

enum ParserError {
    ParserErrorUnexpectedIdentifier,
    ParserErrorExpectedIdentifier,
    ParserErrorUnexpectedIndent,
};

inline const char* getMessageForParserError(ParserError parserError) {
    return parserError == ParserErrorUnexpectedIdentifier    ? "Did not expect identifer." :
           parserError == ParserErrorExpectedIdentifier      ? "Expected identifier." :
           parserError == ParserErrorUnexpectedIndent        ? "Did not expect indent." :
           (assert(false), "");
}

class IIssueReporter {
public:
    virtual void report(rownumber row, colnumber col, LexerError lexerError) = 0;
    virtual void report(rownumber row, colnumber col, ParserError parserError) = 0;
};
