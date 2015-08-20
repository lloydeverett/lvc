//
//  main.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/05/16.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include <iostream>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/exception/all.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include "stringreader.h"
#include "lexer.h"
#include "getfilecontents.h"
#include "ostreamissuereporter.h"
#include "directtokeninputstream.h"
#include "parser.h"
#include "compiler.h"
#include "llvmirgenerator.h"
#include "semanticanalysisconfig.h"

#warning TODO: make all exceptions have .what()
#warning TODO: https://caesr.uwaterloo.ca/misc/boost-llvm-clang.html

int main(int argc, const char * argv[]) {
    boost::filesystem::path path;
    
    if (argc > 1) {
        path = boost::filesystem::path(argv[1]);
    }
    else {
        std::cout << "No input file specified." << '\n';
        std::cout << "Trying to compile main.v in working directory..." << '\n' << '\n';
        path = boost::filesystem::path("main.v");
    }
    
    try {
        std::cout << boost::filesystem::canonical(path) << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }
    
#warning todo: test
#warning TODO: canonical.
    
    std::string fileContents;
    try {
        fileContents = getFileContents(path.native().c_str());
    }
    catch (std::exception e) {
        std::cerr << "error:: " << e.what() << std::endl;
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
    
    llvm::Module* lModule = new llvm::Module(moduleName, llvm::getGlobalContext());
    LLVMIRGenConfig lconfig;
    lconfig.targetModule = lModule;
    
    SemanticAnalysisConfig smconfig;
    smconfig.defaultIntBitWidth = 32;
    smconfig.defaultIntIsSigned = true;
    smconfig.defaultFloatingPointIsDouble = false;
    smconfig.allowImplicitIntUpcast = true;
    
    Compiler<LLVMIRGenerator<DontPreserveNames>> compiler;
    
    ASTCompilationConfig aconfig;
    aconfig.allowImplicitIntegerUpcast = true;
    aconfig.assumedFloatingPointLiteralIsDouble = true;
    aconfig.assumedIntegerLiteralIsSigned = true;
    aconfig.assumedIntegerLiteralNumBits = 32;
    
    ASTCompiler<LLVMIRGenerator<DoPreserveNames>> compiler(aconfig, lconfig);
    compiler.compileFunctionDecls(&astModule);
    compiler.compileFunctionDefinitions(&astModule);
    
    lModule->dump();
    
    return EXIT_SUCCESS;
}
