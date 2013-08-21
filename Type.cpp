//
//  Type.cpp
//  TypeParser
//
//  Created by Eric Alford on 10/30/12.
//  Copyright (c) 2012 Eric Alford. All rights reserved.
//

#include "Type.h"

bool Type::addAlias(string name){
    int i;
    bool flag = true;
    for(i = 0; i<aliases.size(); i++){
        if(name == aliases[i]){
            flag = false;
        }
    }
    //if the variable was already given a type, don't add it to the list. Otherwise, add it.
    
    if(flag)
        aliases.push_back(name);
    
    return flag;
}

bool Type::addVariable(string name){
    int i;
    bool flag = true;
    
    for(i = 0; i<variables.size(); i++){
        if(name == variables[i]){
            flag = false;
        }
    }
    
    if(flag)
        variables.push_back(name);
    
    return flag;
}