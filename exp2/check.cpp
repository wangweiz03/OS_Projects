#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#define FILENAME "processed.txt"

int main() {
    ifstream inflow(FILENAME);
    vector<int> arr;
    int temp;
    while (inflow >> temp) arr.push_back(temp);

    for (int i=1; i<arr.size(); ++i) if (arr[i]<arr[i-1]) {
        cout << "排序有误！" << endl;
        return 0;
    }

    cout << "排序正确！" << endl;
}