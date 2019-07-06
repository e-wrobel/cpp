//
//  main.cpp
//  pointers
//
//  Created by Marcin Wróblewski on 06/07/2019.
//  Copyright © 2019 Marcin Wróblewski. All rights reserved.
//
#define SIZE 1000000
#include <iostream>
#include <iomanip>

using namespace std;

/** Increasing each value by 1 and return array copy*/
int* add_one(int tab[]){

    int *tmp = new int[SIZE];
    
    for (int i = 0; i < SIZE; i++){
        tmp[i] = tab[i] + 1;
    }
    
    return tmp;
}

/** Increasing in-place each value by 1*/
void add_one_ptr(int* tab){
    for (int i = 0; i < SIZE; i++){
        tab[i] = tab[i] + 1;
    }
}

int main(int argc, const char * argv[]) {
    
    clock_t start, end;
    
    // Two arrays, first one is dynamic another is static
    int *tab1 = new int[SIZE];
    int tab2[SIZE];
    
    // Filling data
    // 1. Dynamic
    start = clock();
    
    for (int i = 0; i < SIZE; i++){
        tab1[i] = rand();
    }
    
    end = clock();
    cout << "Dynamic array filed in [s]: " << double(end-start)/CLOCKS_PER_SEC << setprecision(4) << endl;
    
    // Filling data
    // 2. Static
    start = clock();
    
    for (int i = 0; i < SIZE; i++){
        tab2[i] = rand();
    }
    
    end = clock();
    cout << "Static array filed in [s]: " << double(end-start)/CLOCKS_PER_SEC << setprecision(4) << endl;
    
    cout << "Arrays are prepared...\n" << endl;
    
    // Dynamic array manipulation -> takes pointer to original tab and makes changes in-place
    start = clock();
    add_one_ptr(tab1);
    end = clock();
    
    cout << "Dynamic array modified in [s]: " << double(end-start)/CLOCKS_PER_SEC << setprecision(4) << endl;

    // Static array manipulation -> takes original int[] tab as input and returns pointer to new, modified tab
    start = clock();
    add_one(tab2);
    end = clock();
    
    cout << "Static array modified in [s]: " << double(end-start)/CLOCKS_PER_SEC << setprecision(4) << endl;
    
    
    return 0;
}
