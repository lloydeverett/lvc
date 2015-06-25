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

enum LoggingSetting {
    LoggingEnabled,
    LoggingDisabled,
};

class CerrIssueReporter : public IIssueReporter {
private:
    std::string path;
    LoggingSetting loggingSetting;
    bool hasErrorOccuredBool;
    
public:
    CerrIssueReporter(std::string path, LoggingSetting s = LoggingEnabled) : path(path), loggingSetting(s), hasErrorOccuredBool(false) {}
    
    virtual void report(SourcePosition pos, std::string message, Subsystem subsystem) override {
        report(pos.row, pos.col, message, subsystem);
    }
    
    virtual void report(rownumber row, colnumber col, std::string message, Subsystem subsystem) override {
        hasErrorOccuredBool = true;
        std::cerr << path << ":" << row << ":" << col << ": " << "error: " << message
                   << " (" << (subsystem == SubsystemLexer ? "lex" : "parse") << " error)" << std::endl;
    }
    
    virtual bool hasAnErrorOccured() override {
        return hasErrorOccuredBool;
    }
    
};
