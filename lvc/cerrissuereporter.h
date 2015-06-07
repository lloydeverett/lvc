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
    std::string path;
    
public:
    CerrIssueReporter(std::string path) : path(path) {}
    
    virtual ReportedLexerError report(rownumber row, colnumber col, LexerError lexerError) override {
        std::cerr << path << ":" << row << ":" << col << ": error:" << getMessageForLexerError(lexerError) << " (lex error)" << std::endl;
        return makeReportedError(lexerError);
    }
    
    virtual ReportedParserError report(rownumber row, colnumber col, ParserError parserError) override {
        std::cerr << path << ":" << row << ":" << col << ": error:" << getMessageForParserError(parserError) << " (parse error)" << std::endl;
        return makeReportedError(parserError);
    }
        
    virtual void log(std::string s) override {
        std::cout << "LOG: " << s << std::endl;
    }
    
};
