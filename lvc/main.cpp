//
//  main.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/05/16.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include <iostream>
#include "stringreader.h"
#include "lexer.h"
#include "getfilecontents.h"

int main(int argc, const char * argv[]) {
    std::string filePath;
    if (argc > 1) {
        filePath = std::string(argv[1]);
    }
    else {
        std::cout << "No input file specified" << std::endl;
        std::cout << "Trying to compile main.v in working directory..." << std::endl << std::endl;
        filePath = "main.v";
    }
    
    std::string fileContents;
    try {
        fileContents = getFileContents(filePath.c_str());
    }
    catch (std::exception e) {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }
    catch (int e) {
        std::cerr << "error: " << strerror(e) << std::endl;
        return 1;
    }
    
#warning IMPLEMENT
    
    return 0;
}
