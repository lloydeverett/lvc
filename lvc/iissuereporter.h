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
    ParserErrorExpectedIndent,
};

inline const char* getMessageForParserError(ParserError parserError) {
    return parserError == ParserErrorUnexpectedIdentifier    ? "Did not expect identifer." :
           parserError == ParserErrorExpectedIdentifier      ? "Expected identifier." :
           parserError == ParserErrorUnexpectedIndent        ? "Did not expect indent." :
           parserError == ParserErrorExpectedIndent          ? "Expected indent." :
           (assert(false), "");
}

class ReportedLexerError {
    friend class IIssueReporter;
protected:
    ReportedLexerError(LexerError error) : error(error) {}
public:
    const LexerError error;
};

class ReportedParserError {
    friend class IIssueReporter;
protected:
    ReportedParserError(ParserError error) : error(error) {}
public:
    const ParserError error;
};

class IIssueReporter {
protected:
    static ReportedLexerError makeReportedError(LexerError lexerError) {
        return ReportedLexerError(lexerError);
    }
    static ReportedParserError makeReportedError(ParserError parserError) {
        return ReportedParserError(parserError);
    }
public:
    virtual ReportedLexerError report(rownumber row, colnumber col, LexerError lexerError) = 0;
    virtual ReportedParserError report(rownumber row, colnumber col, ParserError parserError) = 0;
    virtual void log(std::string s) = 0;
};
