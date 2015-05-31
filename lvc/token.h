//
//  token.h
//  lvc
//
//  Created by Lloyd Everett on 2015/05/17.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <iostream>
#include "integertypedefs.h"

enum TokenKind {
    Newline,
    Identifier,
    OpenParenthesis,
    CloseParenthesis,
    Int,
    Comment,
    Return,
    Indent,
    Dedent,
    IntegerLiteral,
    RealLiteral,
};

inline TokenKind getTokenKindOfIdentifierOrKeyword(const std::string &str) {
    return str == "return" ? Return :
                             Identifier;
}

inline bool isStrFilledForTokenKind(TokenKind kind) {
    return kind == Identifier || kind == IntegerLiteral || kind == RealLiteral;
}

inline const char* strFromTokenKind(TokenKind kind) {
    return kind == Newline             ? "Newline" :
           kind == Identifier          ? "Identifier" :
           kind == OpenParenthesis     ? "OpenParenthesis" :
           kind == CloseParenthesis    ? "CloseParenthesis" :
           kind == Int                 ? "Int" :
           kind == Comment             ? "Comment" :
           kind == Return              ? "Return" :
           kind == Indent              ? "Indent" :
           kind == Dedent              ? "Dedent" :
           kind == IntegerLiteral      ? "IntegerLiteral" :
           kind == RealLiteral         ? "RealLiteral" :
           (assert(false), "invalid token kind");
}

class Token {
private:
    TokenKind kind;
    rownumber row;
    colnumber startCol;
    charcount length;
    std::string str;
    
public:
    bool is(TokenKind kind) const { return this->kind == kind; }
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
    void dump() {
        std::cerr << '(' << strFromTokenKind(kind) << ") row:" << getRow() << " col:" << getStartCol() << " len:" << getLength();
        
        if (isStrFilledForTokenKind(kind))
            std::cerr << " str: " << getStr();
        
        std::cerr << std::endl;
    }
};
