//
//  parser.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "ast.h"
#include "ireader.h"
#include "iissuereporter.h"
#include "ilexer.h"

#warning TODO: just work out what everything will look like, top down

Function parseFunction(ILexer &lexer, Token firstToken);
Module parseModule(ILexer &lexer, IIssueReporter &issueReporter);

#warning TODO: try just parsing an expression maybe?
