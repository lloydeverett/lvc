//
//  semanticanalyzerexceptions.h
//  lvc
//
//  Created by Lloyd Everett on 2015/08/10.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <stdexcept>

class SemanticAnalyzerException : public std::exception {
protected:
    SemanticAnalyzerException() {}
};

class SemanticAnalyzerExceptionCouldNotMatchTypes : public SemanticAnalyzerException {};
class SemanticAnalyzerExceptionVariableOfVoidType : public SemanticAnalyzerException {};
class SemanticAnalyzerExceptionIncorrectArguments : public SemanticAnalyzerException {};
