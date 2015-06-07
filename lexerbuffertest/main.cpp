//
//  main.cpp
//  lexerbuffertest
//
//  Created by Lloyd Everett on 2015/06/07.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "testutil.h"

int main(int argc, const char * argv[]) {
    std::string path = getPathForVFile("verysimple.v");
    std::string content = getFileContents(path.c_str());
    
    CerrIssueReporter issueeReporter(path);
    StringReader stringReader(content);
    LexerBuffer l(issueeReporter, stringReader);
    
    try {
        while (true) {
            l.readToken().dump();
            std::cout << "PEEK: ";
            l.peekOneAhead().dump();
            std::cout << "PEEk2: ";
            l.peekTwoAhead().dump();
        }
    }
    catch (FinishedLexingException &e) {
        
    }
    
    return 0;
}
