//
//  token.h
//  lvc
//
//  Created by Lloyd Everett on 2015/05/17.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <iostream>
#include <cassert>
#include "primitivetypeenum.h"
#include "integertypedefs.h"

enum TokenKind {
    Newline,
    Identifier,
    OpenParenthesis,
    CloseParenthesis,
    KeywordReturn,
    KeywordIf,
    Indent,
    Dedent,
    IntegerLiteral,
    RealLiteral,
    Operator,
    PrimitiveTypenameKind,
    INVALID_TOKENKIND,
};

inline const char* getLogStrForTokenKind(TokenKind kind) {
    return kind == Newline               ? "Newline" :
           kind == Identifier            ? "Identifier" :
           kind == OpenParenthesis       ? "OpenParenthesis" :
           kind == CloseParenthesis      ? "CloseParenthesis" :
           kind == KeywordReturn         ? "KeywordReturn" :
           kind == KeywordIf             ? "KeywordIf" :
           kind == Indent                ? "Indent" :
           kind == Dedent                ? "Dedent" :
           kind == IntegerLiteral        ? "IntegerLiteral" :
           kind == RealLiteral           ? "RealLiteral" :
           kind == Operator              ? "Operator" :
           kind == PrimitiveTypenameKind ? "PrimitiveTypenameKind" :
           kind == INVALID_TOKENKIND     ? "INVALID_TOKENKIND" :
           (assert(false), "");
}

inline TokenKind getKeywordTokenKindFromStr(const std::string &str) {
    return str == "return"               ? KeywordReturn :
           str == "if"                   ? KeywordIf :
                                           INVALID_TOKENKIND;
}

class Token {
private:
    TokenKind kind;
    rownumber row;
    colnumber startCol;
    charcount length;
    
    PrimitiveTypeEnum primitiveTypeEnum;
    char operatorChar;
    
    // Str contains the contents of a literal (whether it be a number literal or a string literal), OR,
    // if the token is an identifier, it contains the identifer.
    std::string str;
    
public:
    Token() { clean(); }
    
    void clean() {
        kind = INVALID_TOKENKIND;
        row = 99999;
        startCol = 99999;
        length = 99999;
        primitiveTypeEnum = INVALID_PRIMITIVETYPEENUM;
        operatorChar = '\0';
        str = "";
    }
    
    void dump() {
        std::cout << '(' << getLogStrForTokenKind(kind) << ") row:" << getRow() << " col:" << getStartCol() << " len:" << getLength();
        if (str.length() > 0)
            std::cout << " str: " << getStr();
        if (primitiveTypeEnum != INVALID_PRIMITIVETYPEENUM)
            std::cout << " primitiveTypename: " << stringFromPrimitiveTypeEnum(getPrimitiveTypeEnum());
        if (operatorChar != '\0')
            std::cout << " operatorChar: " << operatorChar;
        std::cout << std::endl;
    }
    
    bool is(TokenKind kind) const { return this->kind == kind; }
    bool isNot(TokenKind kind) const { return this->kind != kind; }
    
    void setKind(TokenKind kind) { this->kind = kind; }
    TokenKind getKind() const { return this->kind; }
    
    void setRow(rownumber row) { this->row = row; }
    rownumber getRow() { return this->row; }
    
    void setStartCol(colnumber startCol) { this->startCol = startCol; }
    colnumber getStartCol() { return this->startCol; }
    
    void setLength(charcount length) { this->length = length; }
    charcount getLength() { return this->length; }
    
    void setStr(const std::string &str) { this->str = str; }
    std::string getStr() { return this->str; }
    
    void setOperatorChar(char c) { this->operatorChar = c; }
    char getOperatorChar() { return this->operatorChar; }
    
    void setPrimitiveTypeEnum(PrimitiveTypeEnum p) { primitiveTypeEnum = p; }
    PrimitiveTypeEnum getPrimitiveTypeEnum() { return primitiveTypeEnum; }
};
