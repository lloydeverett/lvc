//
//  testutil.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/06.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <string>

inline std::string getStrForVFile(std::string s) {
    std::string vfilesDir = std::string(VFILES_DIR);
    std::string filePath = vfilesDir + "/" + s;
    std::string fileStr = getFileContents(filePath.c_str());
}
