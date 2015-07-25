//
//  astcompilerexceptions.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/17.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once

class ASTCompilerException : public std::exception {
protected:
    ASTCompilerException() {}
public:
    ~ASTCompilerException() {}
};

class ASTCompilerExceptionCannotMatchTargetType : public ASTCompilerException {};
class ASTCompilerExceptionNumArgsDoNotMatch : public ASTCompilerException {};
class ASTCompilerExceptionNoReturnValue : public ASTCompilerException {};
