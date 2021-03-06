//
//  main.cpp
//  func_as_param
//
//  Created by Marcin Wróblewski on 08/07/2019.
//  Copyright © 2019 Marcin Wróblewski. All rights reserved.
//

#include <iostream>

using namespace std;

int add(int a, int b){
    
    return a + b;
}

int mul(int a, int b){
    
    return a * b;
}

int printme(int (*f)(int, int), int a, int b){
    int val = f(a, b);
    
    return val;
}

int main(int argc, const char * argv[]) {
    
    int (*fun[])(int, int) = {mul, add};
    
    int val = printme(fun[0], 5, 10);
    
    cout << "(MUL) Val: " << val << endl;
    
    val = printme(fun[1], 5, 10);
    
    cout << "(ADD) Val: " << val << endl;
    
    return 0;
}
