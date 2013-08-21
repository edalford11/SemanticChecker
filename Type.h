//
//  Type.h
//  TypeParser
//
//  Created by Eric Alford on 10/30/12.
//  Copyright (c) 2012 Eric Alford. All rights reserved.
//

#ifndef __TypeParser__Type__
#define __TypeParser__Type__

#include <iostream>
#include <vector>

using namespace std;

class Type{
public:
    int typeNum;
    string typeName;
    bool isImplicit;
    vector<string> aliases;
    vector<string> variables;
    
    Type(int typeNum, string typeName, bool isImplicit){
        this->typeNum = typeNum;
        this->typeName = typeName;
        this->isImplicit = isImplicit;
    }
    ~Type(){}
    
    bool addAlias(string);
    bool addVariable(string);
};

#endif /* defined(__TypeParser__Type__) */
