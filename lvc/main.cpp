//
//  main.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/05/16.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include <iostream>
#include <boost/filesystem/fstream.hpp>
#include <boost/exception/all.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include "stringreader.h"
#include "lexer.h"
#include "getfilecontents.h"
#include "ostreamissuereporter.h"
#include "directtokeninputstream.h"
#include "parser.h"
#include "astcompiler.h"
#include "llvmirgenerator.h"

#warning TODO: make all exceptions have .what()

int main(int argc, const char * argv[]) {
    std::string filePathInputStr;
    if (argc > 1) {
        filePathInputStr = std::string(argv[1]);
    }
    else {
        std::cout << "No input file specified" << std::endl;
        std::cout << "Trying to compile main.v in working directory..." << std::endl << std::endl;
        filePathInputStr = "main.v";
    }
    boost::filesystem::path path;
    try {
        path = boost::filesystem::path(filePathInputStr);
    }
    catch (boost::exception& e) {
        std::cerr << boost::diagnostic_information(e, false /*not verbose*/);
    }
#warning todo: test

    
    #warning TODO: canonical.
    
    std::string fileContents;
    try {
        fileContents = getFileContents(path.native().c_str());
    }
    catch (std::exception e) {
        std::cerr << "error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (int e) {
        std::cerr << "error: " << strerror(e) << std::endl;
        return EXIT_FAILURE;
    }
    
    OstreamIssueReporter issueReporter(path.string(), std::cerr);
    StringReader sourceReader(fileContents);
    
    Lexer lexer(sourceReader, issueReporter);
    DirectTokenInputStream tokenInputStream(lexer);
    Parser parser(tokenInputStream, issueReporter);
    const std::string moduleName = path.filename().string();
    ast::Module astModule(parser.parseModule(moduleName));
    ASTCompiler compiler<LLVMIRGenerator> compiler;
    compiler.compile(astModule);
#warning IMPLEMENT
    
    return EXIT_SUCCESS;
}
