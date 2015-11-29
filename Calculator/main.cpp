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

inline int charToDigit(char c){
    return c - '0';
}

string expr = "3+4*5/6";
string expr0 = "(3+2)*(4-2)/(8+7)";
string expr1 ="9-5+2";
string expr2 ="(9-5)+2";
string expr3 ="9-(5+2)";
string expr4 ="(9-(5+2))*3";

vector<string> infixExpr={
    expr,
    expr0,
    expr1,
    expr2,
    expr3,
    expr4,
};

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

typedef int (*twoOpdFuns)(int, int);
map<const string, twoOpdFuns>twoOpdFunsMap;

inline bool isDigit(char c){
    return c >= '0' && c <= '9';
}

inline bool isOperand(char c){
    //For now
    return isDigit(c);
}

inline bool isOperator(char c){
    return c == '+' || c == '-' || c == '*' || c == '/';
}

vector<string> convertInfixToPostfixExprRecursive(const string& infixExpr){
    vector<string> postFixExpr;
    
    string left;
    string right;
    string op;

    vector<string> rightR;
    vector<string> leftV;
    
    string::const_iterator it = infixExpr.begin();
    for (; it != infixExpr.end(); ) {
        if (isDigit(*it)) {
            left = *it;
            op = *(it + 1);
            postFixExpr.push_back(left);
            
            char r = *(it + 2);
            if (r == '+' || r == '-') {
                right = string(1, r);
                postFixExpr.push_back(right);
                it += 3;
            } else if (r == '*' || r == '/'){
                string rL = infixExpr.substr(2, infixExpr.end()-(it+2));
                right = rL;
                rightR = convertInfixToPostfixExprRecursive( rL );
                postFixExpr.insert(postFixExpr.end(), rightR.begin(), rightR.end());
                
                it++;
            } else if ( r == '(') {
                it++;
            } else if ( r == ')' ) {
                it++;
            }
            
            cout <<"L=" << left <<", op=" << op << ", R=" << right << endl;
            postFixExpr.push_back(op);
            
        } else if (isOperator(*it)) {
            op = *it;
            right = *(it+1);
            
            postFixExpr.push_back(right);
            postFixExpr.push_back(op);
            cout << "operator=" << *it << endl;
            it += 2;
        } else if (*it == '('){
            cout << "left parenthnesses=" << *it << endl;
            it++;
        }

    }
    
//    postFixExpr.insert(postFixExpr.end(), leftV.begin(), leftV.end());
//    postFixExpr.insert(postFixExpr.end(), rightR.begin(), rightR.end());
//    postFixExpr.push_back(op);
    
    return postFixExpr;
}


vector<string> convertInfixToPostfixExpr(const string& infixExpr){
    vector<string> postFixExpr;
    
    string::const_iterator it = infixExpr.begin();
    string left = string(1 , *it);
    postFixExpr.push_back(left);
    
    map<const string, twoOpdFuns>::iterator mapIt;

    for (; it != infixExpr.end(); it++) {
        if ((mapIt = twoOpdFunsMap.find(string(1, *it))) != twoOpdFunsMap.end() ) {
            string right = string(1, *(it+1));
            string optor = string(1, *it);
            
            postFixExpr.push_back(right);
            postFixExpr.push_back(optor);
            it++;
        }
    }
    return postFixExpr;
}


int GetPriority(char c){
    if (c == '+' || c == '-') {
        return 10;
    } else if ( c == '*' || c == '/') {
        return 11;
    } else {
        assert(false);
        return 0;
    }
}

vector<char> convertInfixToPostfixExprStack(const string& infixExpr){
    vector<char> postFixExpr(512);
    vector<char> opStack(32);
    
    string::const_iterator it = infixExpr.begin();
    
    for (; it != infixExpr.end(); it++) {
        if (*it == '(') {
            opStack.push_back(*it);
        }else if  (*it == ')' ) {
            postFixExpr.push_back(opStack.back());
            opStack.pop_back();//pop operator
            opStack.pop_back();//pop '('
        }else if (isOperand(*it)) {
            postFixExpr.push_back(*it);
        }else if(isOperator(*it)) {
            if (!opStack.empty()           &&
                isOperator(opStack.back()) &&
                GetPriority(opStack.back()) >= GetPriority(*it)) {
                postFixExpr.push_back(opStack.back());
                
                opStack.pop_back();
                opStack.push_back(*it);
            }else {
                opStack.push_back(*it);
            }
        }
        
//        for (auto opit=opStack.begin(); opit != opStack.end(); opit++) {
//            cout << *opit << " ";
//        }
//        cout << endl;
    }
    
    postFixExpr.insert(postFixExpr.end(), opStack.rbegin(), opStack.rend());
    
//    for (auto it=postFixExpr.begin(); it != postFixExpr.end(); it++) {
//        cout << *it << " ";
//    }
//    cout << endl;

    return postFixExpr;
}

int calcutorPostfixExpr(vector<string> postfixExpr){
    int ret = 0;
    vector<string>::iterator it = postfixExpr.begin();

    if (postfixExpr.size() == 1) {
        return atoi(postfixExpr.begin()->c_str());
    }
    
    map<const string, twoOpdFuns>::iterator mapIt;
    for ( ; it != postfixExpr.end(); it++) {
        if ((mapIt = twoOpdFunsMap.find(*it)) != twoOpdFunsMap.end()) {
            break;
        }
    }
    
    assert(mapIt != twoOpdFunsMap.end());
    
    ret = mapIt->second(atoi((it-2)->c_str()), atoi((it-1)->c_str()));
    
    cout << *(it-2) << *it << *(it-1) << " = " << ret << endl;

    it++;
    postfixExpr.erase(it-3, it-1);
    postfixExpr[0] = to_string(ret);

    return calcutorPostfixExpr(postfixExpr);
}



int main(int argc, const char * argv[]) {
    twoOpdFunsMap.insert(make_pair(operatorAdd, addOpd));
    twoOpdFunsMap.insert(make_pair(operatorMinus, minusOpd));
    twoOpdFunsMap.insert(make_pair(operatorMult, multOpd));
    twoOpdFunsMap.insert(make_pair(operatorDiv, divOpd));

    for_each(infixExpr.begin(), infixExpr.end(), [](string inFixExp){
        auto postfixExpr =convertInfixToPostfixExprStack(inFixExp);
        for (auto it=postfixExpr.begin(); it != postfixExpr.end(); it++) {
            cout << *it;
        }
        cout << endl;
        
//        int result = calcutorPostfixExpr(postfixExpr);
//        cout << "result = " << result << endl;
    });
    
    return 0;
}
