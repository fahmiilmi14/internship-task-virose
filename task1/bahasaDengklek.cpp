#include <iostream>
#include <string>
using namespace std;

int main (){
    string input;
    cin >> input;
    int len = input.length();
    for (int i = 0; i < len; i++){
        if (input [i]>= 'A' && input[i] <= 'Z') input[i]+= 32;
        else input[i] -= 32;
    }
    cout << input;
}
