//
//  parserexception.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/06.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <stdexcept>
#include "iissuereporter.h"

class ParserException : public std::exception {
private:
    const ParserError error;
    ReportedParserError reportOnCurrentTok(ParserError er);
public:
    ParserException(ReportedParserError reportedError) : error(reportedError.error) {}
    ParserError getAssociatedError() { return error; }
};
