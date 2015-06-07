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
    KeywordVoid,
    KeywordReturn,
    KeywordIf,
    Indent,
    Dedent,
    IntegerLiteral,
    RealLiteral,
    Equals,
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
           kind == KeywordVoid         ? "KeywordVoid" :
           kind == KeywordReturn       ? "KeywordReturn" :
           kind == KeywordIf           ? "If" :
           kind == Indent              ? "Indent" :
           kind == Dedent              ? "Dedent" :
           kind == IntegerLiteral      ? "IntegerLiteral" :
           kind == RealLiteral         ? "RealLiteral" :
           kind == Equals              ? "Equals" :
           (assert(false), "");
}

inline bool isTokenKindATypenameKeyword(TokenKind kind) {
    return kind == KeywordInt || kind == KeywordVoid;
}

inline bool canTokenKindRepresentAType(TokenKind kind) {
    return isTokenKindATypenameKeyword(kind) || kind == Identifier;
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
    std::string getStr() { assert(isStrFilledForTokenKind(kind)); return this->str; }
    bool isTypenameKeyword() {
        return isTokenKindATypenameKeyword(this->kind);
    }
    bool isStrFilled() {
        return isStrFilledForTokenKind(this->kind);
    }
    void dump() {
        if (this->kind == INVALID_KIND) {
            std::cout << "INVALID TOKEN" << std::endl;
            return;
        }
        
        std::cout << '(' << strFromTokenKind(kind) << ") row:" << getRow() << " col:" << getStartCol() << " len:" << getLength();
        if (isStrFilledForTokenKind(kind))
            std::cout << " str: " << getStr();
        std::cout << std::endl;
    }
    bool canRepresentAType() {
        return canTokenKindRepresentAType(kind);
    }
};
