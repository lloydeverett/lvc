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
#include "lexerexceptions.h"

int main(int argc, const char * argv[]) {
    std::string filePath = getPathForVFile("verysimple.v");
    std::string fileStr = getFileContents(filePath.c_str());
    
    StringReader reader(fileStr);
    OstreamIssueReporter issueReporter(filePath, std::cerr);
    Lexer lexer(reader, issueReporter);
    
    charcount indent = 0;
    while (true) {
        Token t;
        
        try {
            t = lexer.lexToken();
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
        else if (t.is(Eof)) {
            break;
        }
    }
    return 0;
}
