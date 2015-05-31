//
//  lexer.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/05/23.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "lexer.h"
#include <cassert>

bool isAlpha(char c) {
    return c >= 'A' && c <= 'z';
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isAlphaOrDigit(char c) {
    return isAlpha(c) || isDigit(c);
}

bool isWhitespace(char c) {
    return c == ' ' || c == '\t';
}

int spacesPerWhitespaceChar(char c) {
    return (c == ' ') ? 1 : 4;
}

bool isValidUnescapedCharLiteralChar(char cc) {
    unsigned char c = (unsigned char)cc;
    return (c >= 32  && c <= 126)   // Basic ascii (space to ~)
        || (c >= 128 && c <= 254);  // Extended Ascii set without non-breaking space (Ç to ■)
}

charcount consumeWhitespace(IReader &reader) {
    int numWhitespaceChars = 0;
    while (isWhitespace(reader.peekChar())) {
        char c = reader.readChar();
        numWhitespaceChars += spacesPerWhitespaceChar(c);
    }
    return numWhitespaceChars;
}

Lexer::Lexer(IReader &reader, const IssueReporter &issueReporter) : reader(reader), issueReporter(issueReporter) {
    indentationStack.push(0);
}

void Lexer::popQueuedDedent(Token &tok) {
    assert(queuedDedents.size() > 0);
    
    QueuedDedent q = queuedDedents.front();
    queuedDedents.pop();
    tok.setKind(Dedent);
    tok.setRow(reader.getRow());
    tok.setStartCol(q.getStartCol());
    tok.setLength(q.getLength());
}

bool Lexer::addDedentsToQueueUntilColnumberIsReached(colnumber col) {
    assert(col < indentationStack.top());
    
    colnumber currentCol;
    do {
        colnumber prevCol = indentationStack.top();
        indentationStack.pop();
        currentCol = indentationStack.top();
        
        colnumber dedentStartCol = currentCol;
        colnumber dedentEndCol = prevCol;
        queuedDedents.emplace(dedentStartCol, dedentEndCol - dedentStartCol);
    } while (currentCol > col);
    
    if (col != currentCol) {
        issueReporter.error(reader.getRow(), reader.getCol(), LexerErrorInvalidIndentation);
        return false;
    }
    
    return true;
}

bool Lexer::readNextToken(Token &tok) {
    
    if (queuedDedents.size() > 0) {
        popQueuedDedent(tok);
        return true;
    }
    
    bool atStartOfRowBeforeConsumingWhitespace = reader.atStartOfRow();
    
    charcount consumedWhitespace = consumeWhitespace(reader);
    
    if (atStartOfRowBeforeConsumingWhitespace) {
        charcount indentationLength = consumedWhitespace;
        
        if (indentationLength > indentationStack.top()) {
            colnumber oldIndent = indentationStack.top();
            indentationStack.push(indentationLength);
            colnumber newIndent = indentationStack.top();
            
            tok.setRow(reader.getRow());
            tok.setStartCol(oldIndent);
            tok.setLength(newIndent - oldIndent);
            tok.setKind(Indent);
            return true;
        }
        else if (indentationLength < indentationStack.top()) {
            bool success = addDedentsToQueueUntilColnumberIsReached(indentationLength);
            if (success) {
                popQueuedDedent(tok);
                return true;
            }
            return false;
        }
        
        // Indentation didn't change; continue to lexing tok
    }
    
    // We've dealt with whitespace now; we can consume a char
    rownumber rowOfC = reader.getRow();
    colnumber colOfC = reader.getCol();
    char c = reader.readChar();
    
    if (c == -1) {
        // Generate dedents at EOF
        if (indentationStack.top() > 0) {
            bool success = addDedentsToQueueUntilColnumberIsReached(0);
            assert(success);
            popQueuedDedent(tok);
            return true;
        }
        return false;
    }
    
    // We can actually lex a token now!
    
    tok.setRow(rowOfC);
    tok.setStartCol(colOfC);
    tok.setLength(1);
    
    /////////////////////
    // Simple, single char tokens
    
    if (c == '\n') {
        tok.setKind(Newline);
        return true;
    }
    if (c == '(') {
        tok.setKind(OpenParenthesis);
        return true;
    }
    if (c == ')') {
        tok.setKind(CloseParenthesis);
        return true;
    }
    
    /////////////////////
    // Other tokens
    
    // Line Comment
    if (c == '/' && reader.peekChar() == '/') {
        reader.readChar();
        
        charcount length = 2;
        while (!reader.eof() && reader.peekChar() != '\n' && reader.peekChar() != '\r') {
            // We use peekChar because we don't want to actually consume the newline
            // We do this because the we want the parser to acknowledge it
            // (after all, if there were no comment, it would've seen the newline)
            // Also, eof is a valid way to end the line comment
            
            reader.readChar();
            length++;
        }
            
        tok.setKind(Comment);
        tok.setLength(length);
        return true;
    }
    
    // Block Comment
    if (c == '/' && reader.peekChar() == '*') {
        reader.readChar();
        charcount length = 2;
        
        while (!reader.eof()) {
            char tempc = reader.readChar();
            length++;
            if (tempc == '*') {
                if (reader.peekChar() == '/') {
                    reader.readChar();
                    length++;
                    
                    tok.setKind(Comment);
                    tok.setLength(length);
                    return true;
                }
            }
        }
        
        issueReporter.fatalError(rowOfC, colOfC, LexerFatalErrorUnterminatedBlockComment);
        return false;
    }
    
    // Carriage Return Newline
    if (c == '\r') {
        if (reader.peekChar() != '\n') {
            issueReporter.error(rowOfC, colOfC, LexerErrorInvalidNewline);
            return false;
        }
        
        reader.readChar(); // read the \n char
        
        tok.setKind(Newline);
        tok.setLength(2);
        return true;
    }
    
    // Identifier or Keyword
    if (isAlpha(c)) {
        std::string str = std::string() + c;
        while (isAlphaOrDigit(reader.peekChar())) {
            str += reader.readChar();
        }
        
        tok.setLength(str.length());
        
        tok.setKind(getTokenKindOfIdentifierOrKeyword(str));
        
        if (tok.is(Identifier)) {
            tok.setStr(str);
        }
        
        return true;
    }
    
    // Number Literal
    if (isDigit(c)) {
        std::string str = std::string() + c;
        unsigned int decimalPoints = 0;
        while (isDigit(reader.peekChar() || reader.peekChar() == '.')) {
            if (reader.peekChar() == '.')
                decimalPoints++;
            str += reader.readChar();
        }
        
        if (decimalPoints > 1) {
            issueReporter.error(rowOfC, colOfC, LexerErrorInvalidNumberLiteral);
        }
        
        tok.setKind(decimalPoints == 0 ? IntegerLiteral : RealLiteral);
        tok.setLength(str.length());
        tok.setStr(str);
        return true;
    }
    
    tok.setKind((TokenKind)100);
    return false;
    
#warning TODO: char literals, etc
    
    return false;
}

bool Lexer::isFinished() {
    return reader.eof() && indentationStack.top() == 0 && queuedDedents.empty();
}
