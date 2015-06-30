//
//  main.cpp
//  irtest
//
//  Created by Lloyd Everett on 2015/06/13.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include <iostream>
#include "parser.h"
#include "testutil.h"
#include "irgenerator.h"

int main(int argc, const char * argv[]) {
    std::string path = getPathForVFile("verysimple.v");
    CerrIssueReporter cr(path);
    StringReader s(getFileContents(path.c_str()));
    Parser p(s, cr);
    ast::Module m = p.parse("verysimple");
    IRGenConfig config;
    config.bitsUsedByBooleanType = 8;
    config.preserveNamesInIR = true;
    llvm::Module llvmm("helloworld", llvm::getGlobalContext());
    IRGenVisitor v(&llvmm, config);
    m.accept(v);
    llvmm.dump();
    return 0;
}
