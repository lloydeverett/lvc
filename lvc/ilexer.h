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

class ILexer {
public:
    virtual bool isFinished() = 0;
    virtual Token lexToken() = 0;
    virtual ~ILexer() {}
};
