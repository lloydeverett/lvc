//
//  testutil.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/06.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <string>

// Test includes
#include "getfilecontents.h"
#include "stringreader.h"
#include "lexer.h"
#include "stringreader.h"
#include "cerrissuereporter.h"
#include "parser.h"

inline std::string getPathForVFile(std::string s) {
    std::string vfilesDir = std::string(VFILES_DIR);
    return vfilesDir + "/" + s;
}
