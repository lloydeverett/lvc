//
//  ilexer.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/03.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "token.h"
#include "iissuereporter.h"
#include "lexerexceptions.h"

class ILexer {
public:
    // Returns true if there are no tokens left to produce.
    // (note that this doesn't always correspond with EOF due to dedent tokens).
    virtual bool isFinished() = 0;
    
    // If lexToken successfully lexes a token OR finds a valid substitution for the token, it should return that token.
    // If the function fails and cannot find a substitution, it should throw a LexerErrorException.
    // If isFinished() returns true and this function is still called it should throw a LexerFinishedException.
    // Any issues that were encountered (including substitutions) should be reported to the issueReporter.
    virtual Token lexToken() = 0;
    
    virtual ~ILexer() {}
};
