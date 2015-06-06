//
//  cerrerrorreporter.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <iostream>
#include "iissuereporter.h"

class CerrIssueReporter : public IIssueReporter {
private:
    std::string filename;
    
public:
    CerrIssueReporter(std::string filename) : filename(filename) {}
    
    virtual void report(rownumber row, colnumber col, LexerError lexerError) override {
        std::cerr << filename << ":" << row << ":" << col << ": error:" << getMessageForLexerError(lexerError) << " (lex error)" << std::endl;
    }
    
    virtual void report(rownumber row, colnumber col, ParserError parserError) override {
        std::cerr << filename << ":" << row << ":" << col << ": error:" << getMessageForParserError(parserError) << " (parse error)" << std::endl;
    }
    
};
