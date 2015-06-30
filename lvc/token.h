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
#include <boost/optional.hpp>

enum TokenKind {
    Newline,
    Identifier,
    OpenParenthesis,
    CloseParenthesis,
    Return,
    If,
    Else,
    Struct,
    Indent,
    Dedent,
    IntegerLiteral,
    RealLiteral,
    Plus,
    Minus,
    Asterisk,
    Slash,
    Percent,
    Dot,
    Void,
    Char,
    Short,
    Int,
    Long,
    Uchar,
    Ushort,
    Uint,
    Ulong,
    Float,
    Double,
    Bool,
    Equals,
    EqualsEquals,
    LargerThan,
    SmallerThan,
    LargenThanEquals,
    SmalerThanEquals,
    ExclamationEquals,
    Exclamation,
    Comma,
    Extern,
    Eof,

    INVALID_TOKEN_KIND_VALUE,
};

inline const char* debugStringForTokenKind(TokenKind kind) {
    static const char* vals[] = {
        "Newline",
        "Identifier",
        "OpenParenthesis",
        "CloseParenthesis",
        "Return",
        "If",
        "Else",
        "Struct",
        "Indent",
        "Dedent",
        "IntegerLiteral",
        "RealLiteral",
        "Plus",
        "Minus",
        "Asterisk",
        "Slash",
        "Percent",
        "Dot",
        "Void",
        "Char",
        "Short",
        "Int",
        "Long",
        "Uchar",
        "Ushort",
        "Uint",
        "Ulong",
        "Float",
        "Double",
        "Bool",
        "Equals",
        "EqualsEquals",
        "LargerThan",
        "SmallerThan",
        "LargenThanEquals",
        "SmalerThanEquals",
        "ExclamationEquals",
        "Exclamation",
        "Comma",
        "Extern",
        "Eof",
    };

    int numVals = sizeof(vals) / sizeof(vals[0]);
    assert(kind >= 0 && kind < numVals);
    return vals[kind];
}

class Token {
private:
    TokenKind kind;
    rownumber row;
    colnumber startCol;
    charcount length;

    // Str contains the contents of a literal (whether it be a number literal or a string literal), OR,
    // if the token is an identifier, it contains the identifer.
    std::string str;

public:
    Token() { clean(); }

    void clean() {
        kind = INVALID_TOKEN_KIND_VALUE;
        row = 99999;
        startCol = 99999;
        length = 99999;
        str = "";
    }

    virtual std::ostream& dump(std::ostream& o) const {
        o << "(" << debugStringForTokenKind(kind) << ") row:" << getRow() << " col:" << getStartCol() << " len:" << getLength();
        if (str.length() > 0)
            o << " str: " << getStr();
        return o;
    }

    bool is(TokenKind kind) const { return this->kind == kind; }
    bool isNot(TokenKind kind) const { return this->kind != kind; }

    void setKind(TokenKind kind) { this->kind = kind; }
    TokenKind getKind() const { return this->kind; }

    void setRow(rownumber row) { this->row = row; }
    rownumber getRow() const { return this->row; }

    void setStartCol(colnumber startCol) { this->startCol = startCol; }
    colnumber getStartCol() const { return this->startCol; }

    void setLength(charcount length) { this->length = length; }
    charcount getLength() const { return this->length; }

    void setStr(const std::string &str) { this->str = str; }
    std::string getStr() const { return this->str; }
};

inline std::ostream& operator<<(std::ostream& o, const Token& t) { return t.dump(o); }
