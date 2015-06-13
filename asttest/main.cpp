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

void createAndDestoryALotOfReturnsStmts() {
    std::vector<ast::ReturnStmt> returnStmts;
    for (int i = 0; i < 10000; i++) {
        returnStmts.emplace_back(std::unique_ptr<ast::IExp>(new ast::IntegerLiteralExp("3")));
    }
}

int main(int argc, const char * argv[]) {
    std::cout << argv[0] << std::endl;
    std::cout << "Before" << std::endl;
    createAndDestoryALotOfReturnsStmts();
    std::cout << "After" << std::endl;
    return 0;
}
