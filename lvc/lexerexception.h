//
//  lexerexception.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/06.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <stdexcept>

class LexerException : public std::exception {
private:
    const LexerError error;
    
public:
    LexerException(ReportedLexerError reportedError) : error(reportedError.error) {}
    LexerError getError() { return error; }
    
};

class FinishedLexingException : public std::exception {
    
};
