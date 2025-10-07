#include <iostream>
using namespace std;

int main (){
    int a, b;
    cin >> a >> b;
    int hasil = a*a + b*b +1;
    if (hasil % 4 == 0) cout << (hasil/4);
    else cout <<-1;
}
