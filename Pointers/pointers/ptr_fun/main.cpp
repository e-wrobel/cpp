//
//  main.cpp
//  ptr_fun
//
//  Created by Marcin Wróblewski on 08/07/2019.
//  Copyright © 2019 Marcin Wróblewski. All rights reserved.
//
#define ROW 20000
#define COL 20000

#include <iostream>
#include <unistd.h>

using namespace std;

int printer(int age, string name){
    
    char **tab = new char*[ROW];
    
    // Create COLxROW 2d Array
    for (int i = 0; i< COL; i++){
        tab[i] = new char[COL];
    }
    
    // Fill array with random data
    
    for (int i = 0; i < COL; i++){
        for (int j = 0; j < ROW; j++){
            tab[i][j] = rand();
        }
    }
    
    cout << "Name: " << name << ", age: " << age << endl;
    
    return age;
}

int main(int argc, const char * argv[]) {
    
    int (*func_ptr) (int, string);
    func_ptr = printer;
    
    cout << "Using function pointers" << endl;
    
    func_ptr(40, "Marcin");
    func_ptr(31, "Gosia");
    
    cout << "Using functions" << endl;
    
    // Memory usage 790MB
    
    printer(10, "Adam");
    printer(20, "Wojtek");
    
    cout << "Hit any key to exit\n" << endl;
    
    // Memory usage 1.54GB
    
    int a;
    cin >> a;
    
    return 0;
}
