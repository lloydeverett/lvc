//
//  ireader.h
//  lvc
//
//  Created by Lloyd Everett on 2015/05/23.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "sourceposition.h"

class IReader {
public:
    virtual bool atWhitespace() = 0;
    virtual int readChar() = 0; // returns -1 if EOF
    virtual int peekChar(charcount n = 0) = 0; // returns -1 if EOF
    virtual charcount consumeWhitespace() = 0;
    virtual charcount consumeUntilPositionAtNewlineOrEof() = 0;
    virtual bool eof() = 0;
    virtual rownumber getRow() = 0;
    virtual colnumber getCol() = 0;
    virtual SourcePosition getSourcePosition() = 0;
    virtual bool atStartOfRow() = 0;
    
};
