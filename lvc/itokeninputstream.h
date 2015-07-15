//
//  itokeninputstream.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/05.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "token.h"

class ITokenInputStream {
public:
    virtual Token readToken() = 0;
    virtual Token peekNext() = 0;
    virtual Token peekNextNext() = 0;
    virtual bool isFinished() = 0;
    virtual ~ITokenInputStream() {}
};
