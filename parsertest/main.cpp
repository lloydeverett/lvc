//
//  main.cpp
//  parsertest
//
//  Created by Lloyd Everett on 2015/06/06.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include <iostream>
#include "parser.h"
#include "testutil.h"

int main(int argc, const char * argv[]) {
    std::string filePath = getPathForVFile("verysimple.v");
    std::string fileStr = getFileContents(filePath.c_str());
    
    StringReader s(fileStr);
    CerrIssueReporter c(filePath);
    Parser parser(s, c);
    
    ast::Module m = parser.parse("eh");
    
    std::cout << m;
    
    return 0;
}
