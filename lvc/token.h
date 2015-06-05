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
#include "integertypedefs.h"

enum TokenKind {
    Newline,
    Identifier,
    OpenParenthesis,
    CloseParenthesis,
    KeywordInt,
    KeywordReturn,
    KeywordIf,
    Comment,
    Indent,
    Dedent,
    IntegerLiteral,
    RealLiteral,
    INVALID_KIND,
};

inline TokenKind getTokenKindOfIdentifierOrKeyword(const std::string &str) {
    return str == "int"                ? KeywordInt :
           str == "return"             ? KeywordReturn :
           str == "if"                 ? KeywordIf :
                                         Identifier;
}

inline const char* strFromTokenKind(TokenKind kind) {
    return kind == Newline             ? "Newline" :
           kind == Identifier          ? "Identifier" :
           kind == OpenParenthesis     ? "OpenParenthesis" :
           kind == CloseParenthesis    ? "CloseParenthesis" :
           kind == KeywordInt          ? "KeywordInt" :
           kind == KeywordReturn       ? "KeywordReturn" :
           kind == KeywordIf           ? "If" :
           kind == Comment             ? "Comment" :
           kind == Indent              ? "Indent" :
           kind == Dedent              ? "Dedent" :
           kind == IntegerLiteral      ? "IntegerLiteral" :
           kind == RealLiteral         ? "RealLiteral" :
           (assert(false), "");
}

inline bool isTokenKindATypenameKeyword(TokenKind kind) {
    return kind == KeywordInt;
}

inline bool isStrFilledForTokenKind(TokenKind kind) {
    return kind == Identifier || kind == IntegerLiteral || kind == RealLiteral;
}

class Token {
private:
    TokenKind kind;
    rownumber row;
    colnumber startCol;
    charcount length;
    std::string str;
    
public:
    Token() { this->kind = INVALID_KIND; }
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
    bool isTypenameKeyword() {
        return isTokenKindATypenameKeyword(this->kind);
    }
    void dump() {
        std::cout << '(' << strFromTokenKind(kind) << ") row:" << getRow() << " col:" << getStartCol() << " len:" << getLength();
        if (isStrFilledForTokenKind(kind))
            std::cout << " str: " << getStr();
        std::cout << std::endl;
    }
};
