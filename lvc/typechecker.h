//
//  typechecker.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/30.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once

class TypeChecker {
    
    void openLocalsScope();
    void addLocal();
    void closeLocalsScope();
};
