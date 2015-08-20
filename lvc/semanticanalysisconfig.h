//
//  semanticanalysisconfig.h
//  lvc
//
//  Created by Lloyd Everett on 2015/08/01.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once

struct SemanticAnalysisConfig {
    int defaultIntBitWidth;
    bool defaultIntIsSigned;
    bool defaultFloatingPointIsDouble;
    bool allowImplicitIntUpcast;
};
