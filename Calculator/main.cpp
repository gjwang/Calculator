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


string expr = "3+4*5/6";
string expr0 = "(3+2)*(4-2)/(8+7)";
string expr1 = "9-5+2";
string expr2 = "(9-5)+2";
string expr3 = "9-(5+2)";
string expr4 = "(9-(5+2))*3";

vector<string> infixExprVec={
    expr,
    expr0,
    expr1,
    expr2,
    expr3,
    expr4,
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

inline bool isDigit(char c){
    return c >= '0' && c <= '9';
}

inline int charToDigit(char c){
    return c - '0';
}

inline bool isOperand(char c){
    //For now
    return isDigit(c);
}

inline bool isOperator(char c){
    return c == '+' || c == '-' || c == '*' || c == '/';
}

bool isOperator(const BiOpdFunsMap_t& map, const string& s){
    return map.find(s) != map.end();

}

//vector<string> convertInfixToPostfixExprRecursive(const string& infixExpr){
//    vector<string> postFixExpr;
//    
//    string left;
//    string right;
//    string op;
//
//    vector<string> rightR;
//    vector<string> leftV;
//    
//    string::const_iterator it = infixExpr.begin();
//    for (; it != infixExpr.end(); ) {
//        if (isDigit(*it)) {
//            left = *it;
//            op = *(it + 1);
//            postFixExpr.push_back(left);
//            
//            char r = *(it + 2);
//            if (r == '+' || r == '-') {
//                right = string(1, r);
//                postFixExpr.push_back(right);
//                it += 3;
//            } else if (r == '*' || r == '/'){
//                string rL = infixExpr.substr(2, infixExpr.end()-(it+2));
//                right = rL;
//                rightR = convertInfixToPostfixExprRecursive( rL );
//                postFixExpr.insert(postFixExpr.end(), rightR.begin(), rightR.end());
//                
//                it++;
//            } else if ( r == '(') {
//                it++;
//            } else if ( r == ')' ) {
//                it++;
//            }
//            
//            cout <<"L=" << left <<", op=" << op << ", R=" << right << endl;
//            postFixExpr.push_back(op);
//            
//        } else if (isOperator(*it)) {
//            op = *it;
//            right = *(it+1);
//            
//            postFixExpr.push_back(right);
//            postFixExpr.push_back(op);
//            cout << "operator=" << *it << endl;
//            it += 2;
//        } else if (*it == '('){
//            cout << "left parenthnesses=" << *it << endl;
//            it++;
//        }
//
//    }
//    
////    postFixExpr.insert(postFixExpr.end(), leftV.begin(), leftV.end());
////    postFixExpr.insert(postFixExpr.end(), rightR.begin(), rightR.end());
////    postFixExpr.push_back(op);
//    
//    return postFixExpr;
//}


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
    }
    
    postFixExpr.insert(postFixExpr.end(), opStack.rbegin(), opStack.rend());
    
//    for (auto it=postFixExpr.begin(); it != postFixExpr.end(); it++) {
//        cout << *it << " ";
//    }
//    cout << endl;

    return postFixExpr;
}

int calcutorPostfixExpr(const vector<string>& postfixExpr){
    int ret = 0;
    vector<string> opdStack(32); //operand stack
    vector<string> opStack(32);  //operator stack
    
    for_each(postfixExpr.begin(), postfixExpr.end(), [&](string s){
        if (isOperator(BiOpdFunsMap, s)){
            auto opd1 = opdStack.back();
            opdStack.pop_back();
            auto opd2 = opdStack.back();
            opdStack.pop_back();
            auto it = BiOpdFunsMap.find(s);
            if (it != BiOpdFunsMap.end()) {
                auto operand = it->second(atoi(opd2.c_str()), atoi(opd1.c_str()));
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

    for_each(infixExprVec.begin(), infixExprVec.end(), [](string inFixExp){
        for_each(inFixExp.begin(), inFixExp.end(), [](char c){
            cout << c;
        });
        
        cout << " = ";
        
        auto postfixExpr =convertInfixToPostfixExprStack(inFixExp);
        
        for_each(postfixExpr.begin(), postfixExpr.end(), [](char c){
            cout << c;
        });

        //cout << endl;
 
        vector<string> postfixExprString;
        for_each(postfixExpr.begin(), postfixExpr.end(), [&](char c){
            postfixExprString.push_back(string(1, c));
        });
        
        int ret = calcutorPostfixExpr(postfixExprString);
        cout << " = " << ret << endl;
    });
    
    return 0;
}
