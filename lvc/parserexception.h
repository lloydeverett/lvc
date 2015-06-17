//
//  parserexception.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/06.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <stdexcept>

enum ParserInputError {
    
};

class ParserInputErrorException : public std::exception {
private:
    ParserInputError error;
public:
    ParserInputErrorException(ParserInputError error) : error(error) {
        
    }
    ParserInputError getError() {
        return error;
    }
};
