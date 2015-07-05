//
//  main.cpp
//  symbolstest
//
//  Created by Lloyd Everett on 2015/06/20.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include <iostream>
#include "symbols.h"
#include <memory>
#include <cassert>

int main(int argc, const char * argv[]) {
    Symbols symbols;
    symbols.openBlock();
    sdfgiofvioio;
    std::vector<ast::ArgumentDecl> args;
    ast::FunctionDecl dec(std::make_unique<ast::PrimitiveType>(ast::PrimitiveType(Int)), "hello", std::move(args));
    symbols.addSymbol(&dec);
    assert(*symbols.getSymbol("hello") == &dec);
    symbols.closeBlock();
    return 0;
}
