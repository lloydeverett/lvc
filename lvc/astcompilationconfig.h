//
//  astcompilationconfig.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/16.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once

struct ASTCompilationConfig {
    int assumedIntegerLiteralNumBits;
    bool assumedIntegerLiteralIsSigned;
    bool assumedFloatingPointLiteralIsDouble;
    bool allowImplicitIntegerUpcast;
};
