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
    OstreamIssueReporter cr(path, std::ostream);
    std::string contents = getFileContents(path.c_str());
    StringReader s(contents);
    Parser p(s, cr);
    ast::Module m = p.parse("verysimple");
    IRGenConfig config;
    config.numBitsUsedByBooleans = 8;
    config.preserveNamesInIR = true;
    llvm::Module llvmm("helloworld", llvm::getGlobalContext());
    IRGenerator irGenerator(&llvmm, config);
    irGenerator.genModule(m);
    llvmm.dump();
    return 0;
}
