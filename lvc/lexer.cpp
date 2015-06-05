//
//  lexer.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/04.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "lexer.h"
#include "integertypedefs.h"
#include <cassert>

bool isAlpha(char c) {
    return c >= 'A' && c <= 'z';
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

Token Lexer::getTokenFromQueuedDedent(QueuedDedent q) {
    Token t;
    t.setKind(Dedent);
    t.setStartCol(q.startCol);
    t.setLength(q.length);
    t.setRow(reader.getRow());
    return t;
}

bool Lexer::addDedentsToQueueUntilColnumberIsReached(colnumber col) {
    assert(indentStack.top() > col);
    
    colnumber currentCol;
    do {
        colnumber prevCol = indentStack.top();
        indentStack.pop();
        currentCol = indentStack.top();
        
        colnumber dedentStartCol = currentCol;
        colnumber dedentEndCol = prevCol;
        queuedDedents.emplace(dedentStartCol, dedentEndCol - dedentStartCol);
    } while (currentCol > col);
    
    if (col != indentStack.top()) {
        return false;
    }
    
    return true;
}

Token Lexer::makeIndentToken(colnumber col) {
    assert(col > indentStack.top());
    
    colnumber origIndent = indentStack.top();
    indentStack.push(col);
    Token t;
    t.setKind(Indent);
    t.setRow(reader.getRow());
    t.setStartCol(origIndent);
    t.setLength(col - origIndent);
    return t;
}

Lexer::Lexer(IReader &reader) : reader(reader) {
    indentStack.push(0);
}

bool Lexer::isFinished() {
    return reader.eof() && indentStack.top() == 0 && queuedDedents.size() == 0;
}

Token Lexer::lexToken(IIssueReporter &issueReporter) {
    
    // Return a queued dedent if there are any.
    // This should always get priority over other tokens
    // (and thus the code is run first).
    if (queuedDedents.size() > 0) {
        QueuedDedent q = queuedDedents.front();
        queuedDedents.pop();
        return getTokenFromQueuedDedent(q);
    }
    
    bool atStartOfRowBeforeConsumingWhitespace = reader.atStartOfRow();
    charcount whitespaceCount = reader.consumeWhitespace();
    
    if (atStartOfRowBeforeConsumingWhitespace) {
        if (whitespaceCount > indentStack.top()) {
            return makeIndentToken(whitespaceCount);
        }
        else if (whitespaceCount < indentStack.top()) {
            bool success = addDedentsToQueueUntilColnumberIsReached(whitespaceCount);
            if (!success) {
                issueReporter.report(reader.getRow(), reader.getCol(), LexerErrorInvalidDedent);
                throw LexerException(LexerErrorInvalidDedent);
            }
            // Pop one off the queue
            QueuedDedent q = queuedDedents.front();
            queuedDedents.pop();
            return getTokenFromQueuedDedent(q);
        }
    }
    
    if (reader.eof()) {
        if (indentStack.top() > 0) {
            bool success = addDedentsToQueueUntilColnumberIsReached(0);
            assert(success); // should never fail
            // Pop one off the queue
            QueuedDedent q = queuedDedents.front();
            queuedDedents.pop();
            return getTokenFromQueuedDedent(q);
        }
        else {
            throw FinishedLexingException();
        }
    }
    
    const colnumber colOfC = reader.getCol();
    const colnumber rowOfC = reader.getRow();
    const char c = reader.readChar();
    
    Token t;
    t.setStartCol(colOfC);
    t.setRow(rowOfC);
    
    if (c == '/' && reader.peekChar() == '/') {
        reader.readChar();
        charcount len = 2;
        len += reader.consumeUntilPositionAtNewlineOrEof();
        t.setKind(Comment);
        t.setLength(len);
        return t;
    }
    
#warning TODO: Block Comments (string literals must be acknowledged).
//  if (c == '/' && reader.peekChar() == '*') {
//      reader.readChar();
//      charcount len = 2;
//
//  }
    
    if (c == '\n') {
        t.setKind(Newline);
        t.setLength(1);
        return t;
    }
    
    if (c == '\r') {
        charcount len = 1;
        if (reader.peekChar() == '\n') {
            reader.readChar();
            len = 2;
        }
        t.setKind(Newline);
        t.setLength(len);
        return t;
    }
    
    if (c == '(') {
        t.setKind(OpenParenthesis);
        t.setLength(1);
        return t;
    }
    
    if (c == ')') {
        t.setKind(CloseParenthesis);
        t.setLength(1);
        return t;
    }
    
    if (isAlpha(c)) {
        std::string str;
        str += c;
        while (isAlpha(reader.peekChar()) || isDigit(reader.peekChar())) {
            str += reader.readChar();
        }
        
        t.setKind(getTokenKindOfIdentifierOrKeyword(str));
        if (t.is(Identifier)) {
            t.setStr(str);
        }
        t.setLength(str.length());
        return t;
    }
    
    if (isDigit(c)) {
        std::string str;
        str += c;
        while (isDigit(reader.peekChar())) {
            str += reader.readChar();
        }
        if (isAlpha(reader.peekChar())) {
            issueReporter.report(reader.getRow(), reader.getCol(), LexerErrorInvalidNumberLiteral);
            throw LexerException(LexerErrorInvalidNumberLiteral);
        }
        t.setKind(IntegerLiteral);
        t.setLength(str.length());
        t.setStr(str);
        return t;
    }
    
#warning TODO: Throw exception on unrecognized tokens.
    assert("Unrecognized token");
    return Token();
}

bool Lexer::attemptToRecoverBySkippingLine() {
    return (assert(false), true);
}

bool Lexer::attemptToRecoverBySkippingUntilValidIndentation() {
    return (assert(false), true);
}
