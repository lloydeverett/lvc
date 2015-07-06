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
protected:
    LexerException() {}
};

class LexerErrorException : public LexerException {
protected:
    LexerErrorException() {}
};

class LexerErrorExceptionInvalidDedent : public LexerErrorException {};
class LexerErrorExceptionUnterminatedBlockComment : public LexerErrorException {};
class LexerErrorInvalidNumberLiteral : public LexerErrorException {};
class LexerErrorUnknownCharacter : public LexerErrorException {};

class LexerFinishedException : public LexerException {
    
};
