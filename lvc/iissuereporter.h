//
//  iissuereporter.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "integertypedefs.h"
#include <string>
#include <cassert>
#include "sourceposition.h"

enum Subsystem {
    SubsystemLexer,
    SubsystemParser,
};

class IIssueReporter {
public:
    virtual void report(SourcePosition pos, std::string message, Subsystem subsystem) = 0;
    virtual void report(rownumber row, colnumber col, std::string message, Subsystem subsystem) = 0;
    virtual void log(std::string s) = 0;
    virtual bool hasAnErrorOccured() = 0;
};
