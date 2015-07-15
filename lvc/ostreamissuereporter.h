//
//  ostreamerrorreporter.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <iostream>
#include "iissuereporter.h"

class OstreamIssueReporter : public IIssueReporter {
private:
    std::string path;
    std::ostream& ostream;
    bool hasErrorOccuredBool;
    
public:
    OstreamIssueReporter(std::string path, std::ostream& ostream) : path(path), ostream(ostream), hasErrorOccuredBool(false) {}
    
    virtual void report(SourcePosition pos, std::string message, Subsystem subsystem) override {
        report(pos.row, pos.col, message, subsystem);
    }
    
    virtual void report(rownumber row, colnumber col, std::string message, Subsystem subsystem) override {
        hasErrorOccuredBool = true;
        ostream << path << ":" << row << ":" << col << ": " << "error: " << message
                   << " (" << (subsystem == SubsystemLexer ? "lex" : "parse") << " error)" << '\n';
    }
    
    virtual bool hasAnErrorOccured() override {
        return hasErrorOccuredBool;
    }
    
};
