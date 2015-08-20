//
//  main.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/07/26.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#warning TODO: USE TOOL TO FIX INCLUDES

#include <iostream>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/exception/all.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include "getfilecontents.h"
#include "ostreamissuereporter.h"
#include "stringreader.h"
#include "lexer.h"
#include "directtokeninputstream.h"
#include "parser.h"
#include "ast.h"
#include "llvmirgenconfig.h"
#include "compiler.h"
#include "llvmirgenerator.h"
#include "semanticanalyzer.h"

namespace fs = boost::filesystem;

bool compile(fs::path path) {
    std::string sourceStr;
    std::string canonicalPathStr;
    std::string filenameStr;
    try {
        canonicalPathStr = fs::canonical(path).string();
        std::cout << canonicalPathStr << '\n';
        sourceStr = getFileContents(path.native().c_str());
        filenameStr = path.filename().string();
    }
    catch (std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return false;
    }
    catch (int n) {
        std::cerr << "error: " << strerror(n) << '\n';
        return false;
    }
    
    OstreamIssueReporter issueReporter(canonicalPathStr, std::cerr);
    StringReader stringReader(sourceStr);
    Lexer lexer(stringReader, issueReporter);
    DirectTokenInputStream tokenInputStream(lexer);
    Parser parser(tokenInputStream, issueReporter);
    ast::Module module = parser.parseModule(filenameStr);
    module.dump(std::cerr);
    llvm::Module* lModule = new llvm::Module(filenameStr, llvm::getGlobalContext());
    
    SemanticAnalysisConfig smConfig;
    smConfig.allowImplicitIntUpcast = true;
    smConfig.defaultFloatingPointIsDouble = true;
    smConfig.defaultIntBitWidth = 32;
    smConfig.defaultIntIsSigned = true;
    SemanticAnalyzer sma(smConfig);
    
    LLVMIRGenConfig genConfig;
    genConfig.targetModule = lModule;
    LLVMIRGenerator llvmirg(genConfig);
    
    Compiler<LLVMIRGenerator> compiler(sma, llvmirg);
    compiler.compileLookaheadSymbols(module);
    compiler.compileDefinitions(module);
    
    lModule->dump();
    
    #warning TODO: output instead of dump
    #warning TODO: delete? i dunno
    delete lModule;
    return true;
}

int main(int argc, char* argv[]) {
    fs::path path;
    if (argc > 1) {
        path = fs::path(argv[1]);
    }
    else {
        path = fs::path("main.v");
        if (!fs::exists(path)) {
            std::cout << "No path specified and main.v cannot be found.\n";
            return EXIT_FAILURE;
        }
        else {
            std::cout << "No path specified; compiling main.v.\n";
        }
    }
    
    return compile(path) ? EXIT_SUCCESS : EXIT_FAILURE;
}
