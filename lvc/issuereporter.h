//
//  errorreporter.h
//  lvc
//
//  Created by Lloyd Everett on 2015/05/26.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <iostream>
#include <cassert>
#include "integertypedefs.h"

enum Error {
    LexerErrorInvalidIndentation,
    LexerErrorInvalidNewline,
    LexerErrorInvalidNumberLiteral,
};

inline std::string getMessageForError(Error error) {
    switch (error) {
        case LexerErrorInvalidIndentation:
            return "Indentation is not valid (failed to parse dedent token).";
        case LexerErrorInvalidNewline:
            return "Strange newline found. Newlines should be LF or CRLF.";
        case LexerErrorInvalidNumberLiteral:
            return "Invalid number literal.";
        default:
            assert(false);
    }
}

enum FatalError {
    LexerFatalErrorUnterminatedBlockComment,
};

inline std::string getMessageForFatalError(FatalError fatalError) {
    switch (fatalError) {
        case LexerFatalErrorUnterminatedBlockComment:
            return "Block comment was not terminated.";
        default:
            assert(false);
    }
}

enum Warning {
    
};

inline std::string getMessageForWarning(Warning warning) {
    switch (warning) {
        default:
            assert(false);
    }
}

class IssueReporter {
private:
    std::string filepath;
    std::string parenthetical;
    
public:
    IssueReporter() {}
    IssueReporter(std::string filepath)
    : filepath(filepath) {}
    IssueReporter(std::string filepath, std::string parenthetical)
    : filepath(filepath), parenthetical(parenthetical) {}
    
    void error(rownumber row, colnumber col, Error error) const {
        std::cerr << filepath << ':' << row << ':' << col << ": error: " << getMessageForError(error);
    }
    
    void fatalError(rownumber row, colnumber col, FatalError fatalError) const {
        std::cerr << filepath << ':' << row << ':' << col << ": fatal error: " << getMessageForFatalError(fatalError);
    }
    
    void warning(rownumber row, colnumber col, Warning warning) const {
        std::cerr << filepath << ':' << row << ':' << col << ": warning: " << getMessageForWarning(warning);
    }
};
