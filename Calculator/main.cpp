//
//  main.cpp
//  Calculator
//
//  Created by gjwang on 11/22/15.
//  Copyright (c) 2015 gjwang. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <cassert>

using namespace std;


string expr = "(3+2*5 + (80-79)*2*2)*2";
string expr0 = "(3+2)*(4-2)/(8+7)";
string expr1 = "9-5+2";
string expr2 = "(9-5)+2";
string expr3 = "(9-(5+2))";
string expr4 = "(((9-  ((((5+2))))  )))";
string expr5 = " (9-   ( 20 + 10 ) ) * 2 ";

vector<string> infixExprVec={
    expr,
    expr0,
    expr1,
    expr2,
    expr3,
    expr4,
    expr5,
};

typedef int (*BiOpdFuns)(int, int);
typedef map<const string, BiOpdFuns> BiOpdFunsMap_t;
BiOpdFunsMap_t BiOpdFunsMap;

const string operatorAdd = "+";
const string operatorMinus = "-";
const string operatorMult = "*";
const string operatorDiv = "/";

inline int addOpd(int a, int b){
    return a + b;
}

inline int minusOpd(int a, int b){
    return a - b;
}

inline int multOpd(int a, int b){
    return a * b;
}

inline int divOpd(int a, int b){
    return a / b;
}

inline bool isBlank(const char c){
    return c == ' ';
}

inline bool isDigit(const char c){
    return c >= '0' && c <= '9';
}

inline int charToDigit(const char c){
    return c - '0';
}

inline bool isOperand(const char c){
    //For now
    return isDigit(c);
}

inline bool isOperator(const char c){
    return c == '+' || c == '-' || c == '*' || c == '/';
}

bool isOperator(const BiOpdFunsMap_t& map, const string& s){
    return map.find(s) != map.end();

}

int GetPriority(const char c){
    if (c == '+' || c == '-') {
        return 10;
    } else if ( c == '*' || c == '/') {
        return 11;
    } else {
        assert(false);
        return 0;
    }
}

int GetPriority(const string& s){
    if (s.compare("+") == 0 || s.compare("-") == 0) {
        return 10;
    } else if ( s.compare("*") == 0 || s.compare("/") == 0) {
        return 11;
    } else {
        assert(false);
        return 0;
    }
}

vector<string> preProcessInfixExpr(const string& infixExpr){
    vector<string> infixExprVec;
    string tmp("");
    
    for_each(infixExpr.begin(), infixExpr.end(), [&](char c){
        if (isDigit(c)) {
            tmp.push_back(c);
        }else{
            if (tmp.length() != 0) {
                infixExprVec.push_back(tmp);
                tmp.clear();
            }
            
            if ( !isBlank(c) ) {
                //skip any blank
                infixExprVec.push_back(string(1, c));
            }
        }
    });
    
    //push the last operand into stack
    if (tmp.length() != 0) {
        infixExprVec.push_back(tmp);
    }
    return infixExprVec;
}

vector<string> convertInfixToPostfixExprStack(const vector<string>& infixExpr){
    vector<string> postFixExpr(128);
    vector<string> opStack(32);
    
    vector<string>::const_iterator it = infixExpr.begin();
    
    for (; it != infixExpr.end(); it++) {
        if (it->compare("(") == 0) {
            opStack.push_back(*it);
        }else if (it->compare(")") == 0) {
            //pop all operator within ()
            for (auto it=opStack.rbegin(); it->compare("(") != 0; it++) {
                postFixExpr.push_back(opStack.back());
                opStack.pop_back();//pop operator
            }
            
            opStack.pop_back();//pop "("
        }else if (isOperator(BiOpdFunsMap, *it)) {
            if (!opStack.empty()           &&
                isOperator(BiOpdFunsMap, opStack.back()) &&
                GetPriority(opStack.back()) >= GetPriority(*it)
                ) {
                //meet an operator, it's priority is great_or_equal to the last one's
                postFixExpr.push_back(opStack.back());
                
                opStack.pop_back();
                opStack.push_back(*it);
            }else {
                opStack.push_back(*it);
            }
        }else{
            //if (isOperand(*it)) {
            postFixExpr.push_back(*it);
        }
    }
    
    postFixExpr.insert(postFixExpr.end(), opStack.rbegin(), opStack.rend());
    return postFixExpr;
}

int calcutorPostfixExpr(const vector<string>& postfixExpr){
    int ret = 0;
    vector<string> opdStack(32); //operand stack
    vector<string> opStack(32);  //operator stack
    
    for_each(postfixExpr.begin(), postfixExpr.end(), [&](const string s){
        if (isOperator(BiOpdFunsMap, s)){
            auto opd2 = opdStack.back();
            opdStack.pop_back();
            auto opd1 = opdStack.back();
            opdStack.pop_back();
            auto it = BiOpdFunsMap.find(s);
            if (it != BiOpdFunsMap.end()) {
                auto operand = it->second(stoi(opd1), stoi(opd2));
                //cout << "operand=" << operand << " ";
                opdStack.push_back(to_string(operand));
                ret = operand;
            } else {
                //should not happen
                assert(false);
            }
        }else{
            //s is operand
            opdStack.push_back(s);
        }
    });

    return ret;
}



int main(int argc, const char * argv[]) {
    BiOpdFunsMap.insert(make_pair(operatorAdd, addOpd));
    BiOpdFunsMap.insert(make_pair(operatorMinus, minusOpd));
    BiOpdFunsMap.insert(make_pair(operatorMult, multOpd));
    BiOpdFunsMap.insert(make_pair(operatorDiv, divOpd));

    for_each(infixExprVec.begin(), infixExprVec.end(), [](const string& inFixExp){
        for_each(inFixExp.begin(), inFixExp.end(), [](char c){
            cout << c;
        });
        
        cout << " = ";
        
        auto infixExprString = preProcessInfixExpr(inFixExp);
        auto postfixExpr =convertInfixToPostfixExprStack(infixExprString);
        
        for_each(postfixExpr.begin(), postfixExpr.end(), [](const string& expr){
            cout << expr;
        });

        //cout << endl;
        
        int ret = calcutorPostfixExpr(postfixExpr);
        cout << " = " << ret << endl;
    });
    
    return 0;
}
