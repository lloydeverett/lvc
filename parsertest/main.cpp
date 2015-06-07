//
//  main.cpp
//  parsertest
//
//  Created by Lloyd Everett on 2015/06/06.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include <iostream>
#include "testutil.h"

int main(int argc, const char * argv[]) {
    std::string filePath = getPathForVFile("verysimple.v");
    std::string fileStr = getFileContents(filePath.c_str());
    
    StringReader s(fileStr);
    CerrIssueReporter c(filePath);
    Parser parser(s, c);
    
    parser.parseModule();
    
    return 0;
}
