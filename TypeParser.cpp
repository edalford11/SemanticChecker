//
//  TypeParser.cpp
//  TypeParser
//
//  Created by Eric Alford on 10/30/12.
//  Copyright (c) 2012 Eric Alford. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Type.h"

using namespace std;

string fileArray[10000];
string relop[5] = {"<",">","<=",">=","<>"};
vector<Type> types;
vector<string> declTypes, declVars, implicitTypes, explicitTypes, implicitVariables, explicitVariables;
int i, j, k, l, fileIndex=0, size = 0;
int typeNumber = 4;
bool multiTypeDecl, typeDeclVar, multiVarDecl, typeMismatch = false;

void printErrors(){
    if(multiTypeDecl){
        cout << "ERROR CODE 0" << endl;
        exit(1);
    }
    if(typeDeclVar){
        cout << "ERROR CODE 1" << endl;
        exit(1);
    }
    if(multiVarDecl){
        cout << "ERROR CODE 2" << endl;
        exit(1);
    }
    if(typeMismatch){
        cout << "ERROR CODE 3" << endl;
        exit(1);
    }
}

string getNumType(const string &s){
    string numType = "false";
    bool flag = true;
    bool period = false;
    string::const_iterator it = s.begin();
    for(it=s.begin(); it<s.end(); it++){
        if(!isdigit(*it) && *it != '.'){
            flag = false;
            numType = "NOTNUM";
        }
        if(*it == '.' && period){   //if there is more than one period, not a number.
            numType = "NOTNUM";
            flag = false;
        }
        if(flag && *it == '.' && !period){ //if this is the first period
            numType = "REAL";
            period = true;
        }
    }
    
    if(!period && flag){
        numType = "INT";
    }
    return numType;
}

bool isOperator(int op){
    bool flag = false;
    if(op == '+' | op == '-' | op == '*' | op == '/' | op == '=')
        flag = true;
    return flag;
}

void getFile(){
    char c = getchar();
    string word = "";
    
    while(!feof(stdin)){
        word = "";
        while(isspace(c) && !feof(stdin)){
            c = getchar();
        }
        while(!isspace(c) && !feof(stdin) && c != ';' && !isOperator(c) && c != ',' && c != '(' && c != ')'){
            word += c;
            c = getchar();
        }
        if(word != ""){
            fileArray[size] = word;
            size++;
        }
        if(c == ';' || isOperator(c)){
            fileArray[size] = c;
            size++;
            c = getchar();
        }
        else
            c = getchar();
    }
}

void parseType(){
    int index;
    vector<string> typeList;
    while(fileArray[fileIndex] != "VAR" && fileArray[fileIndex] != "{"){
        typeList.clear();
        index = -1;
        while(fileArray[fileIndex] != ":"){ //add all variables to list if there are multiple on a single line.
            typeList.push_back(fileArray[fileIndex]);
            fileIndex++;
        }
        
        fileIndex++; //skip the : character
        string type2 = fileArray[fileIndex];
        fileIndex = fileIndex+2; //skip the semicolon.
        
        for(i = 0; i<declTypes.size(); i++){    //make sure the type wasn't already declared.
            for(j=0; j<typeList.size(); j++){
                if(declTypes[i] == typeList[j]){
                    multiTypeDecl = true;
                    return;
                }
            }
        }
        
        if(!multiTypeDecl){
            for(i = 0; i<types.size(); i++){    //is the right hand side a built in type?
                if(types[i].typeName == type2){
                    index = i;
                }
            }
            if(index != -1){    //the right hand side is a built in type
                for(j=0; j<typeList.size(); j++){
                    if(types[index].addAlias(typeList[j])){
                        declTypes.push_back(typeList[j]);    //add id to the list of declared types
                        explicitTypes.push_back(typeList[j]);
                    }
                    else{
                        multiTypeDecl = true;  //if the type given was aready declared, we have an error.
                        return;
                    }
                }
            }
            else{   //the right hand side is either an alias or implicitely declared
                bool isAlias = false;
                for(i = 0; i<types.size(); i++){
                    for(j=0; j<types[i].aliases.size(); j++){
                        if(types[i].aliases[j] == type2){
                            isAlias = true;
                            for(k=0; k<typeList.size(); k++){
                                if(types[i].addAlias(typeList[k])){
                                    declTypes.push_back(typeList[k]);    //add id to the list of declared types
                                    explicitTypes.push_back(typeList[k]);
                                }
                                else{
                                    multiTypeDecl = true;  //if the type given was aready declared, we have an error.
                                    return;
                                }
                            }
                        }
                    }
                }
                if(!isAlias){   //the right hand side is an implicitely declared type.
                    Type *newType = new Type(typeNumber, type2, true);
                    typeNumber++;
                    implicitTypes.push_back(type2);
                    declTypes.push_back(type2); //add type2 to the list of declared types
                    for(j=0; j<typeList.size(); j++){
                        explicitTypes.push_back(typeList[j]);
                        newType->addAlias(typeList[j]);
                        declTypes.push_back(typeList[j]);    //add id to the list of declared types
                    }
                    types.push_back(*newType);
                }
            }
        }
    }
}

void parseVar(){
    int index;
    vector<string> varList;
    while((fileArray[fileIndex] != "TYPE" && fileArray[fileIndex] != "{")){
        varList.clear();
        index = -1;
        while(fileArray[fileIndex] != ":"){ //add all variables to list if there are multiple on a single line.
            varList.push_back(fileArray[fileIndex]);
            fileIndex++;
        }
        
        fileIndex++; //skip the : character
        string type2 = fileArray[fileIndex];    //type2 is the right side.
        fileIndex = fileIndex+2; //skip the semicolon.
        
        for(i = 0; i<declVars.size(); i++){     //make sure the var(s) wasn't already declared.
            for(j=0; j<varList.size(); j++){
                if(declVars[i] == varList[j]){
                    multiVarDecl = true;
                    return;
                }
            }
        }
        for(i = 0; i<declTypes.size(); i++){    //make sure the var(s) isn't already a type name.
            for(j=0; j<varList.size(); j++){
                if(declTypes[i] == varList[j]){
                    typeDeclVar = true;
                    return;
                }
            }
        }
        
        if(!multiVarDecl){
            for(i = 0; i<types.size(); i++){    //is the right hand side a built in type?
                if(types[i].typeName == type2){
                    index = i;
                }
            }
            
            if(index != -1){     //the right hand side is a built in type
                for(j=0; j<varList.size(); j++){
                    if(types[index].addVariable(varList[j])){
                        declVars.push_back(varList[j]);    //add id to the list of declared vars
                        explicitVariables.push_back(varList[j]);
                    }
                    else{
                        multiVarDecl = true;  //if the var given was aready declared, we have an error.
                        return;
                    }
                }
            }
            else{       //the right hand side is either an alias or implicitely declared
                bool isAlias = false;
                for(i = 0; i<types.size(); i++){
                    for(j=0; j<types[i].aliases.size(); j++){
                        if(types[i].aliases[j] == type2){
                            isAlias = true;
                            for(k=0; k<varList.size(); k++){
                                if(types[i].addVariable(varList[k])){
                                    declVars.push_back(varList[k]);    //add id to the list of declared vars
                                    explicitVariables.push_back(varList[k]);
                                }
                                else{
                                    multiVarDecl = true;  //if the var given was aready declared, we have an error.
                                    return;
                                }
                            }
                        }
                    }
                }
                if(!isAlias){   //the right hand side is an implicitely declared type.
                    Type *newType = new Type(typeNumber, type2, true);
                    typeNumber++;
                    implicitTypes.push_back(type2);    //only add the right side to the list of implicit types/vars.
                    for(j=0; j<varList.size(); j++){
                        explicitVariables.push_back(varList[j]);
                        newType->addVariable(varList[j]);
                        declVars.push_back(varList[j]);    //add id to the list of declared vars
                    }
                    declTypes.push_back(type2); //add type2 to the list of declared types
                    types.push_back(*newType);
                }
            }
        }
    }
}

int getType(string id){
    int t = -1;
    bool implicit = true;
    int k, l;
    for(k = 0; k<declVars.size(); k++){ //check to see if the var was declared.
        if(id == declVars[k]){
            implicit = false;
        }
    }
    
    if(implicit){   //if it's not a variable, make sure it isn't a declared type;
        for(k=0; k<declTypes.size(); k++){
            if(id == declTypes[k])
                t = -2;         //a -2 will indicate it is a type, return error in parseBody() function.
        }
    }
    else{
        for(k=0; k<types.size(); k++){
            for(l=0; l<types[k].variables.size(); l++){
                if(types[k].variables[l] == id){
                    t = k;
                }
            }
        }
    }
    return t;
}

void dropCopyType(int in, int drop){
    if(in != drop){ //make sure they are not the same type
        if(types[drop].typeName != ""){
            types[in].aliases.push_back(types[drop].typeName);
        }
        for(i=0; i<types[drop].aliases.size(); i++){
            types[in].aliases.push_back(types[drop].aliases[i]);
        }
        for(i=0; i<types[drop].variables.size(); i++){
            types[in].variables.push_back(types[drop].variables[i]);
        }
        types.erase(types.begin()+drop);    //erases the element at index drop.
    }
}

bool isOperator(string op){
    bool flag = false;
    if(op == "+" | op == "-" | op == "*" | op == "/")
        flag = true;
    return flag;
}

void parseWhileExpression(){
    int rightType;
    fileIndex++;
    //evaluate the while expression.
    string leftSide = fileArray[fileIndex];
    int leftType = getType(leftSide);   //get the type number for the left hand side.
    if(leftType == -2){      //-2 indicates the left side is a type id, we have an error.
        typeDeclVar = true;
        printErrors();
    }
    else{
        string isNumLeft = getNumType(fileArray[fileIndex]);
        fileIndex++;
        if(fileArray[fileIndex] == "{"){    //no relop, only one id in while expression
            if(isNumLeft != "NOTNUM"){  //a number is not allowed in this case.
                typeMismatch = true;
                printErrors();
            }
            if(leftType != -1 && leftType <= 3){    //leftside is not implicitely declared
                if(leftType != 3){
                    typeMismatch = true;
                    printErrors();
                }
            }
            else if(leftType == -1){     //the left side is an implicit variable, turn it into a boolean
                declVars.push_back(leftSide);
                types[3].variables.push_back(leftSide);
                implicitVariables.push_back(leftSide);
            }
            else{       //leftType > 3, implicitely declared in the VAR or BODY section
                dropCopyType(3, leftType);
            }
            fileIndex--;
        }
        else{   //must be a relop
            bool isRelop = false;
            for(i=0; i<5; i++){
                if(relop[i] == fileArray[fileIndex]){
                    isRelop = true;
                }
            }
            if(!isRelop){
                cout << "Error. Relation Operator Expected" << endl;
                exit(1);
            }
            
            fileIndex++;
            rightType = getType(fileArray[fileIndex]);
            string isNumRight = getNumType(fileArray[fileIndex]);
            if(rightType == -2){      //-2 indicates the right side is a type id, we have an error.
                typeDeclVar = true;
                printErrors();
            }
            else if(leftType != -1 && leftType <= 3){    //left side is not implicitely declared
                if(rightType != -1 && rightType <=3){   //right side is not implicitely declared
                    if(leftType != rightType){
                        typeMismatch = true;
                        printErrors();
                    }
                }
                else if(rightType == -1){       //right side is either a number or implicitely declared
                    if(isNumRight == "NOTNUM"){
                        declVars.push_back(fileArray[fileIndex]);
                        implicitVariables.push_back(fileArray[fileIndex]);
                        types[leftType].variables.push_back(fileArray[fileIndex]);
                    }
                    else if(isNumRight == "REAL"){
                        if(leftType != 1){
                            typeMismatch = true;
                            printErrors();
                        }
                    }
                    else{   //isNumRight is INT
                        if(leftType != 0){
                            typeMismatch = true;
                            printErrors();
                        }
                    }
                }
                else{   //rightType is > 3, implicitely declared with an implicit type.
                    dropCopyType(leftType, rightType);
                }
            }
            else if(leftType == -1){    //left side is either a number or implicitely declared
                if(isNumLeft == "NOTNUM"){      //left side is implicitely declared
                    if(rightType != -1 && rightType <=3){    //right side is not implicitely declared
                        declVars.push_back(leftSide);
                        implicitVariables.push_back(leftSide);
                        types[rightType].variables.push_back(leftSide);
                    }
                    
                    else if(rightType == -1){       //right side is either a number or implicitely declared
                        if(isNumRight == "NOTNUM"){ //right side is implicitely declared
                            Type *newType = new Type(typeNumber, "", true);
                            typeNumber++;
                            newType->addVariable(leftSide);
                            newType->addVariable(fileArray[fileIndex]);
                            declVars.push_back(leftSide);
                            declVars.push_back(fileArray[fileIndex]);
                            implicitVariables.push_back(leftSide);
                            implicitVariables.push_back(fileArray[fileIndex]);
                            types.push_back(*newType);
                        }
                        else if(isNumRight == "REAL"){
                            declVars.push_back(leftSide);
                            implicitVariables.push_back(leftSide);
                            types[1].variables.push_back(leftSide);
                        }
                        else{    //isNumRight is INT
                            declVars.push_back(leftSide);
                            implicitVariables.push_back(leftSide);
                            types[0].variables.push_back(leftSide);
                        }
                    }
                    else{   //rightType is > 3, implicitely declared in the VAR or BODY section.
                        declVars.push_back(leftSide);
                        implicitVariables.push_back(leftSide);
                        types[rightType].variables.push_back(leftSide);
                    }
                }
                else if(isNumLeft == "REAL"){   //left side is a decimal number
                    if(rightType != -1 && rightType <=3){   //right side is not implicitely declared
                        if(rightType != 1){
                            typeMismatch = true;
                            printErrors();
                        }
                    }
                    else if(rightType == -1){       //right side is either a number or implicitely declared
                        if(isNumRight == "NOTNUM"){ //right side is implicitely declared
                            declVars.push_back(fileArray[fileIndex]);
                            implicitVariables.push_back(fileArray[fileIndex]);
                            types[1].variables.push_back(fileArray[fileIndex]);
                        }
                        else if(isNumRight == "REAL"){
                            //do nothing
                        }
                        else{    //isNumRight is INT
                            typeMismatch = true;
                            printErrors();
                        }
                    }
                    else{   //rightType is > 3, implicitely declared in the VAR or BODY section.
                        dropCopyType(1, rightType);
                    }
                }
                else{   //isNumLeft is an INT
                    if(rightType != -1 && rightType <=3){   //right side is not implicitely declared
                        if(rightType != 0){
                            typeMismatch = true;
                            printErrors();
                        }
                    }
                    else if(rightType == -1){       //right side is either a number or implicitely declared
                        if(isNumRight == "NOTNUM"){ //right side is implicitely declared
                            declVars.push_back(fileArray[fileIndex]);
                            implicitVariables.push_back(fileArray[fileIndex]);
                            types[0].variables.push_back(fileArray[fileIndex]);
                        }
                        else if(isNumRight == "REAL"){
                            typeMismatch = true;
                            printErrors();
                        }
                        else{    //isNumRight is INT
                            //do nothing
                        }
                    }
                    else{   //rightType is > 3, implicitely declared in the VAR or BODY section.
                        dropCopyType(0, rightType);
                    }
                }
            }
            else{       //leftType > 3, implicitely declared in the VAR or BODY section.
                if(rightType != -1 && rightType <=3){   //right side is not implicitely declared
                    dropCopyType(rightType, leftType);
                }
                else if(rightType == -1){       //right side is either a number or implicitely declared
                    if(isNumRight == "NOTNUM"){
                        declVars.push_back(fileArray[fileIndex]);
                        implicitVariables.push_back(fileArray[fileIndex]);
                        types[leftType].variables.push_back(fileArray[fileIndex]);
                    }
                    else if(isNumRight == "REAL"){
                        dropCopyType(1, leftType);
                    }
                    else{   //isNumRight is INT
                        dropCopyType(0, leftType);
                    }
                }
                else{   //rightType is > 3, implicitely declared with an implicit type.
                    dropCopyType(leftType, rightType);
                }
            }
        }
    }
}

void parseWhileBody(){
    int leftType, rightType, indexTrack = 0;
    while(fileArray[fileIndex] != "}"){
        string leftSide = fileArray[fileIndex];
        if(leftSide == "WHILE"){
            parseWhileExpression();
            fileIndex = fileIndex+2;    //skip the { character
            parseWhileBody();
            fileIndex++;
        }
        else{
            leftSide = fileArray[fileIndex];
            leftType = getType(leftSide);   //get the type number for the left hand side.
            if(leftType == -2){      //-2 indicates the left side is a type id, we have an error.
                typeDeclVar = true;
                printErrors();
            }
            fileIndex++;
            if(fileArray[fileIndex] == "="){
                fileIndex++;
                
                
                while(fileArray[fileIndex] != ";"){
                    if(indexTrack%2 > 0){   //if indexTrack is odd. fileIndex must be an operator.
                        if(!isOperator(fileArray[fileIndex])){
                            cout << "Error. Operator Expected" << endl;
                            exit(1);
                        }
                        fileIndex++;
                        indexTrack++;
                    }
                    else{       //not an operator
                        string isNum = getNumType(fileArray[fileIndex]);
                        if(leftType != -1 && leftType <= 3){ //leftside is not implicitely declared
                            if(isNum == "NOTNUM"){
                                rightType = getType(fileArray[fileIndex]);
                                if(rightType == -2){      //-2 indicates the right side is a type id, we have an error.
                                    typeDeclVar = true;
                                    printErrors();
                                }
                                if(rightType != -1 && !types[rightType].isImplicit){//right side is not
                                    if(rightType != leftType){                      //implicitely declared
                                        typeMismatch = true;
                                        printErrors();
                                    }
                                }
                                //right side is implicitely declared with an implicit type
                                else if(rightType != -1 && types[rightType].isImplicit){
                                    dropCopyType(leftType, rightType);
                                }
                                else{                   //right side is implicitely declared
                                    for(l=0; l<types.size(); l++){
                                        if(types[l].typeNum == leftType)
                                            types[l].variables.push_back(fileArray[fileIndex]);
                                    }
                                }
                            }
                            else if(isNum == "REAL"){
                                if(leftType != 1){
                                    typeMismatch = true;
                                    printErrors();
                                }
                            }
                            else{   //isNum is type INT
                                if(leftType != 0){
                                    typeMismatch = true;
                                    printErrors();
                                }
                            }
                            fileIndex++;
                            indexTrack++;
                        }
                        else if(leftType == -1){   //left side is an implicitely declared variable
                            if(isNum == "NOTNUM"){
                                int rightType = getType(fileArray[fileIndex]);
                                if(rightType == -2){      //-2 indicates the right side is a type id, we have an error.
                                    typeDeclVar = true;
                                    printErrors();
                                }
                                if(rightType != -1){    //if rightType is not implicitely declared
                                    for(l=0; l<types.size(); l++){
                                        if(types[l].typeNum == rightType){
                                            implicitVariables.push_back(leftSide);  //add leftSide to implicit variable list
                                            types[l].addVariable(leftSide);
                                            declVars.push_back(leftSide); //add the left side to the declVars list since it now is defined.
                                            leftType = getType(leftSide);   //update the left side type for further analyzation.
                                        }
                                    }
                                }
                                //right side is implicitely declared with an implicit type
                                else if(rightType != -1 && types[rightType].isImplicit){
                                    implicitVariables.push_back(leftSide);  //add leftSide to implicit variable list
                                    types[rightType].addVariable(leftSide);
                                    declVars.push_back(leftSide);   //add the left side to the declVars list since it now is defined.
                                    leftType = getType(leftSide);   //update the left side type for further analyzation.
                                }
                                else{       //right side is implicitely declared
                                    Type *newType = new Type(typeNumber, "", true);
                                    typeNumber++;
                                    newType->addVariable(leftSide);
                                    declVars.push_back(leftSide);
                                    implicitVariables.push_back(leftSide);
                                    if(leftSide != fileArray[fileIndex]){
                                        newType->addVariable(fileArray[fileIndex]);
                                        declVars.push_back(fileArray[fileIndex]);
                                        implicitVariables.push_back(fileArray[fileIndex]);
                                    }
                                    types.push_back(*newType);
                                    leftType = getType(leftSide);   //update the left side type for further analyzation.
                                }
                            }
                            else if(isNum == "REAL"){
                                implicitVariables.push_back(leftSide);
                                types[1].addVariable(leftSide);
                                declVars.push_back(leftSide);   //add the left side to the declVars list since it now is defined.
                                leftType = getType(leftSide);   //update the left side type for further analyzation.
                            }
                            else{   //isNum is type INT
                                implicitVariables.push_back(leftSide);
                                types[0].addVariable(leftSide);
                                declVars.push_back(leftSide);   //add the left side to the declVars list since it now is defined.
                                leftType = getType(leftSide);   //update the left side type for further analyzation.
                            }
                            fileIndex++;
                            indexTrack++;
                        }
                        else{   //leftType > 3, implicitely declared in the VAR or BODY section
                            if(isNum == "NOTNUM"){
                                //update according to the else statement in the leftside implicit section (if type name is nothing)
                                int rightType = getType(fileArray[fileIndex]);
                                if(rightType == -2){      //-2 indicates the right side is a type id, we have an error.
                                    typeDeclVar = true;
                                    printErrors();
                                }
                                if(rightType != -1 && !types[rightType].isImplicit){//right side is not implicitely declared
                                    dropCopyType(rightType, leftType);
                                    leftType = getType(leftSide);   //update the left side type for further analyzation.
                                }
                                //right side is implicitely declared with an implicit type
                                else if(rightType != -1 && types[rightType].isImplicit){
                                    if(types[leftType].typeName != "")
                                        dropCopyType(leftType, rightType);
                                    else
                                        dropCopyType(rightType, leftType);
                                    leftType = getType(leftSide);   //update the left side type for further analyzation.
                                }
                                else{                   //right side is implicitely declared
                                    types[leftType].addVariable(fileArray[fileIndex]);
                                    declVars.push_back(fileArray[fileIndex]);
                                    implicitVariables.push_back(fileArray[fileIndex]);
                                }
                            }
                            else if(isNum == "REAL"){
                                dropCopyType(1, leftType);      //the left slide is already in the declVars list.
                                leftType = getType(leftSide);   //update the left side type for further analyzation.
                            }
                            else{   //isNum is type INT
                                dropCopyType(0, leftType);      //the left slide is already in the declVars list.
                                leftType = getType(leftSide);   //update the left side type for further analyzation.
                            }
                            fileIndex++;
                            indexTrack++;
                        }
                    }
                }
                fileIndex++;    //move fileIndex to the next position after the ";"
                indexTrack = 0;
            }
            else{
                cout << "Error. = Expected." << endl;
                exit(1);
            }
        }
    }
}

void parseBody(){
    int leftType, rightType, indexTrack = 0;
    while(fileArray[fileIndex] != "}"){
        string leftSide = fileArray[fileIndex];
        if(leftSide == "WHILE"){
            parseWhileExpression();
            fileIndex = fileIndex+2;    //skip the { character
            parseWhileBody();
            fileIndex++;
        }
        else{
            leftSide = fileArray[fileIndex];
            leftType = getType(leftSide);   //get the type number for the left hand side.
            if(leftType == -2){      //-2 indicates the left side is a type id, we have an error.
                typeDeclVar = true;
                return;
            }
            fileIndex++;
            if(fileArray[fileIndex] == "="){
                fileIndex++;
                
                
                while(fileArray[fileIndex] != ";"){
                    if(indexTrack%2 > 0){   //if indexTrack is odd. fileIndex must be an operator.
                        if(!isOperator(fileArray[fileIndex])){
                            cout << "Error. Operator Expected" << endl;
                            exit(1);
                        }
                        fileIndex++;
                        indexTrack++;
                    }
                    else{       //not an operator
                        string isNum = getNumType(fileArray[fileIndex]);
                        if(leftType != -1 && leftType <= 3){ //leftside is not implicitely declared
                            if(isNum == "NOTNUM"){
                                rightType = getType(fileArray[fileIndex]);
                                if(rightType == -2){      //-2 indicates the right side is a type id, we have an error.
                                    typeDeclVar = true;
                                    return;
                                }
                                if(rightType != -1 && !types[rightType].isImplicit){//right side is not
                                    if(rightType != leftType){                      //implicitely declared
                                        typeMismatch = true;
                                        return;
                                    }
                                }
                                //right side is implicitely declared with an implicit type
                                else if(rightType != -1 && types[rightType].isImplicit){
                                    dropCopyType(leftType, rightType);
                                }
                                else{                   //right side is implicitely declared
                                    for(l=0; l<types.size(); l++){
                                        if(types[l].typeNum == leftType)
                                            types[l].variables.push_back(fileArray[fileIndex]);
                                    }
                                }
                            }
                            else if(isNum == "REAL"){
                                if(leftType != 1){
                                    typeMismatch = true;
                                    return;
                                }
                            }
                            else{   //isNum is type INT
                                if(leftType != 0){
                                    typeMismatch = true;
                                    return;
                                }
                            }
                            fileIndex++;
                            indexTrack++;
                        }
                        else if(leftType == -1){   //left side is an implicitely declared variable
                            if(isNum == "NOTNUM"){
                                int rightType = getType(fileArray[fileIndex]);
                                if(rightType == -2){      //-2 indicates the right side is a type id, we have an error.
                                    typeDeclVar = true;
                                    return;
                                }
                                if(rightType != -1){    //if rightType is not implicitely declared
                                    for(l=0; l<types.size(); l++){
                                        if(types[l].typeNum == rightType){
                                            implicitVariables.push_back(leftSide);  //add leftSide to implicit variable list
                                            types[l].addVariable(leftSide);
                                            declVars.push_back(leftSide); //add the left side to the declVars list since it now is defined.
                                            leftType = getType(leftSide);   //update the left side type for further analyzation.
                                        }
                                    }
                                }
                                //right side is implicitely declared with an implicit type
                                else if(rightType != -1 && types[rightType].isImplicit){
                                    implicitVariables.push_back(leftSide);  //add leftSide to implicit variable list
                                    types[rightType].addVariable(leftSide);
                                    declVars.push_back(leftSide);   //add the left side to the declVars list since it now is defined.
                                    leftType = getType(leftSide);   //update the left side type for further analyzation.
                                }
                                else{       //right side is implicitely declared
                                    Type *newType = new Type(typeNumber, "", true);
                                    typeNumber++;
                                    newType->addVariable(leftSide);
                                    declVars.push_back(leftSide);
                                    implicitVariables.push_back(leftSide);
                                    if(leftSide != fileArray[fileIndex]){
                                        newType->addVariable(fileArray[fileIndex]);
                                        declVars.push_back(fileArray[fileIndex]);
                                        implicitVariables.push_back(fileArray[fileIndex]);
                                    }
                                    types.push_back(*newType);
                                    leftType = getType(leftSide);   //update the left side type for further analyzation.
                                }
                            }
                            else if(isNum == "REAL"){
                                implicitVariables.push_back(leftSide);
                                types[1].addVariable(leftSide);
                                declVars.push_back(leftSide);   //add the left side to the declVars list since it now is defined.
                                leftType = getType(leftSide);   //update the left side type for further analyzation.
                            }
                            else{   //isNum is type INT
                                implicitVariables.push_back(leftSide);
                                types[0].addVariable(leftSide);
                                declVars.push_back(leftSide);   //add the left side to the declVars list since it now is defined.
                                leftType = getType(leftSide);   //update the left side type for further analyzation.
                            }
                            fileIndex++;
                            indexTrack++;
                        }
                        else{   //leftType > 3, implicitely declared in the VAR or BODY section
                            if(isNum == "NOTNUM"){
                                int rightType = getType(fileArray[fileIndex]);
                                if(rightType == -2){      //-2 indicates the right side is a type id, we have an error.
                                    typeDeclVar = true;
                                    return;
                                }
                                if(rightType != -1 && !types[rightType].isImplicit){//right side is not implicitely declared
                                    dropCopyType(rightType, leftType);
                                    leftType = getType(leftSide);   //update the left side type for further analyzation.
                                }
                                //right side is implicitely declared with an implicit type
                                else if(rightType != -1 && types[rightType].isImplicit){
                                    if(types[leftType].typeName != ""){
                                        dropCopyType(leftType, rightType);
                                    }
                                    else
                                        dropCopyType(rightType, leftType);
                                    leftType = getType(leftSide);   //update the left side type for further analyzation.
                                }
                                else{                   //right side is implicitely declared
                                    types[leftType].addVariable(fileArray[fileIndex]);
                                    declVars.push_back(fileArray[fileIndex]);
                                    implicitVariables.push_back(fileArray[fileIndex]);
                                }
                            }
                            else if(isNum == "REAL"){
                                dropCopyType(1, leftType);      //the left slide is already in the declVars list.
                                leftType = getType(leftSide);   //update the left side type for further analyzation.
                            }
                            else{   //isNum is type INT
                                dropCopyType(0, leftType);      //the left slide is already in the declVars list.
                                leftType = getType(leftSide);   //update the left side type for further analyzation.
                            }
                            fileIndex++;
                            indexTrack++;
                        }
                    }
                }
                fileIndex++;    //move fileIndex to the next position after the ";"
                indexTrack = 0;
            }
            else{
                cout << "Error. = Expected." << endl;
                exit(1);
            }
        }
    }
}

int main(){
    //initialize the built in types.
    Type *integerType = new Type(0, "INT", false);
    Type *realType = new Type(1, "REAL", false);
    Type *stringType = new Type(2, "STRING", false);
    Type *booleanType = new Type(3, "BOOLEAN", false);
    types.push_back(*integerType);
    types.push_back(*realType);
    types.push_back(*stringType);
    types.push_back(*booleanType);
    
    getFile();
    
    /*
    for(i=0; i<size; i++){
        cout << fileArray[i] << " ";
    }
    cout << endl;*/

    if(fileArray[fileIndex] == "TYPE"){
        fileIndex++;
        parseType();
    }
    if(fileArray[fileIndex] == "VAR"){
        fileIndex++;
        parseVar();
    }
    if(fileArray[fileIndex] == "{"){
        fileIndex++;
        parseBody();
    }
    else{
        cout << "Error. VAR, TYPE or { expected at beginning of input." << endl;
        return 0;
    }
    
    printErrors();
    
    for(i = 0; i<types.size(); i++){
        if(types[i].typeName != ""){
            if(types[i].aliases.size()!=0 || types[i].variables.size()!=0)
                cout << types[i].typeName << " : ";
            for(j=0; j<explicitTypes.size(); j++){      //print the explicit types in the order they were introduced.
                for(k=0; k<types[i].aliases.size(); k++){
                    if(explicitTypes[j] == types[i].aliases[k])
                        cout << types[i].aliases[k] << " ";
                }
            }
            for(j=0; j<implicitTypes.size(); j++){      //print the implicit types in the order they were introduced.
                for(k=0; k<types[i].aliases.size(); k++){
                    if(implicitTypes[j] == types[i].aliases[k])
                        cout << types[i].aliases[k] << " ";
                }
            }
        
            for(j=0; j<explicitVariables.size(); j++){      //print the explicit variables in the order they were introduced.
                for(k=0; k<types[i].variables.size(); k++){
                    if(explicitVariables[j] == types[i].variables[k])
                        cout << types[i].variables[k] << " ";
                }
            }
            for(j=0; j<implicitVariables.size(); j++){      //print the implicit variables in the order they were introduced.
                for(k=0; k<types[i].variables.size(); k++){
                    if(implicitVariables[j] == types[i].variables[k])
                        cout << types[i].variables[k] << " ";
                }
            }
            if(types[i].aliases.size()!=0 || types[i].variables.size()!=0)
                cout << "#" << endl;
        }
    }
    for(i = 0; i<types.size(); i++){
        if(types[i].typeName == ""){
            cout << types[i].variables[0] << " : " << types[i].variables[1] << " #" << endl;
        }
    }
}