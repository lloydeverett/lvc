//
//  stringreader.h
//  lvc
//
//  Created by Lloyd Everett on 2015/05/23.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <string>
#include <cstdint>
#include <cassert>
#include "ireader.h"

class StringReader : public IReader {
private:
    const std::string &str;
    sourceposition position;
    sourceposition positionAfterLastNewline;
    unsigned int row;
    
public:
    StringReader(const std::string &str) : str(str), position(0), positionAfterLastNewline(0), row(0) {}
    
    virtual bool atWhitespace() override {
        return peekChar(0) == ' ';
    }
    
    virtual int readChar() override {
        if (eof()) return -1;
        
        char c = str[position++];
        if (c == '\n') {
            row++;
            positionAfterLastNewline = position;
        }
        return c;
    }
    
    virtual int peekChar(charcount n) override {
        if (position + n >= str.length()) return -1;
        else return str[position + n];
    }
    
    virtual charcount consumeWhitespace() override {
        charcount count = 0;
        while (!eof() && str[position] == ' ') {
            position++;
            count++;
        }
        return count;
    }
    
    virtual charcount consumeUntilPositionAtNewlineOrEof() override {
        charcount count = 0;
        while (!eof() && str[position] != '\n') {
            position++;
            count++;
        }
        return count;
    }
    
    virtual bool eof() override {
        return position >= str.length();
    }
    
    virtual rownumber getRow() override {
        return row;
    }
    
    virtual colnumber getCol() override {
        return (colnumber)(position - positionAfterLastNewline);
    }
    
    virtual sourceposition getPositionAfterLastNewline() override {
        return positionAfterLastNewline;
    }
    
    virtual sourceposition getPosition() override {
        return position;
    }
    
    virtual bool atStartOfRow() override {
        return positionAfterLastNewline == position;
    }
    
};
