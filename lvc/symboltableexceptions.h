//
//  symboltableexceptions.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/17.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once

class SymbolTableException : public std::exception {
protected:
    SymbolTableException() {}
};

class SymbolTableExceptionVariableAlreadyRegisteredInScope : public SymbolTableException {};
class SymbolTableExceptionFunctionAlreadyRegistered : public  SymbolTableException {};
class SymbolTableExceptionVariableNotFoundUponLookup : public SymbolTableException {};
class SymbolTableExceptionFunctionNotFoundUponLookup : public SymbolTableException {};
