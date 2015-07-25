//
//  main.cpp
//  asttest
//
//  Created by Lloyd Everett on 2015/05/31.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include <iostream>
#include <vector>
#include "ast.h"

int main(int argc, const char * argv[]) {
    
    ast::IntegerType a(8, ast::IntegerType::Signed, Constant);
    ast::IntegerType b(8, ast::IntegerType::Signed, Constant);
    assert(a.isEqual(b));
    
    ast::BoolType c(Constant);
    ast::BoolType d(NotConstant);
    assert(!a.isEqual(d));
    
    ast::FloatingPointType e(ast::FloatingPointType::VariationFloat, NotConstant);
    assert(!e.isEqual(d));
    
    return 0;
}
