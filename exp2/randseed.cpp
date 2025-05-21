#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

#define N 1000000
#define MIN 0
#define MAX 100000000

int main() {
    ofstream out("data.txt");
    if (out.is_open())
        for (int i=0; i<N; ++i)
            out << ((rand()<<10 + rand()) % (MAX - MIN + 1)) + MIN << endl;
    out.close();
}