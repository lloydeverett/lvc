//
//  main.cpp
//  lexertest
//
//  Created by Lloyd Everett on 2015/05/24.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include <iostream>
#include "stringreader.h"
#include "lexer.h"
#include "getfilecontents.h"
#include "stringreader.h"

int main(int argc, const char * argv[]) {
    std::string vfilesDir = std::string(VFILES_DIR);
    std::string filePath = vfilesDir + "/verysimple.v";
    std::string fileStr = getFileContents(filePath.c_str());
    
    StringReader reader(fileStr);
    IssueReporter issueReporter(filePath);
    Lexer lexer(reader, issueReporter);
    
    Token tok;
    while (!lexer.isFinished()) {
        bool result = lexer.readNextToken(tok);
        
        if (result) {
            tok.dump();
        }
        else {
            std::cout << "ERROR_TOK" << std::endl;
        }
    }
    return 0;
}
