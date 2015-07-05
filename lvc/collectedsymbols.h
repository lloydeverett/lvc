//
//  collectedsymbols.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/05.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once

class CollectedSymbols {
    // Collected symbols are decls that we collect
    // during syntax analysis (accomplished by the Parser).
    // Exampls of this are, say, function decls or global variables--
    // anything that can be referred to *before* its decl appears in source.
    // Local variables are not collected.
    
    std::unordered_map<std::string, ast::IDecl*> collected;
    
    void collectIfCollectable(std::unique_ptr<IDecl>& collectableUniquePtr) {
        
    }
};
