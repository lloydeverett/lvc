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
    
};

enum LexerError {
    LexerErrorInvalidDedent,
    LexerErrorUnterminatedBlockComment,
    LexerErrorInvalidNumberLiteral,
};

class LexerErrorException : public LexerException {
private:
    LexerError error;
public:
    LexerErrorException(LexerError error) : error(error) {
        
    }
    LexerError getError() {
        return error;
    }
};

class LexerFinishedException : public LexerException {
    
};
