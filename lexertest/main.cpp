//
//  main.cpp
//  lexertest
//
//  Created by Lloyd Everett on 2015/05/24.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include <iostream>
#include "testutil.h"
#include "lexer.h"

int main(int argc, const char * argv[]) {
    std::string filePath = getPathForVFile("verysimple.v");
    std::string fileStr = getFileContents(filePath.c_str());
    
    StringReader reader(fileStr);
    CerrIssueReporter issueReporter(filePath);
    Lexer lexer(reader);
    
    charcount indent = 0;
    while (!lexer.isFinished(issueReporter)) {
        Token t;
        
        try {
            t = lexer.lexToken(issueReporter);
        }
        catch (LexerErrorException &e) {
            assert(false);
        }
        catch (LexerFinishedException &e) {
            assert(false);
        }
        
        if (t.is(Indent)) {
            indent += 4;
        }
        
        std::cout << std::string(indent, ' ') << t << std::endl;
        
        if (t.is(Dedent)) {
            assert(indent >= 4);
            indent -= 4;
        }
    }
    return 0;
}
