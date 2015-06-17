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
    
};

enum ParserInputError {
    
};

class ParserErrorException : public ParserException {
    
};
