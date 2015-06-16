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
#include "lexerexception.h"

class ILexer {
public:
    
    // Returns true if there are no tokens left to produce.
    // (note that this doesn't always correspond with EOF due to dedent tokens).
    virtual bool isFinished() = 0;
    
    // If lexToken successfully lexes a token OR finds a valid substitution for the token, it should return that token.
    // If the function fails and cannot find a substitution, it should throw a LexException.
    // Any issues that were encountered (including substitutions) should be reported to the issueReporter.
    virtual Token lexToken(IIssueReporter &issueReporter) = 0;
    
    // Attempts to recover when the last call to lexToken resulted in an exception being thrown.
    virtual bool attemptToRecoverBySkippingLine() = 0;
    virtual bool attemptToRecoverBySkippingUntilValidIndentation() = 0;
    
    virtual ~ILexer() {}
    
};
