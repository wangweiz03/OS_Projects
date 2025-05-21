#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
using namespace std;

#define IN_FILENAME "data.txt" // 随机数文件名

int partition(vector<int>& arr, int left, int right){
    int pivot = arr[right];
    int l = left;
    for (int m = left; m < right; ++m)
        if (arr[m] < pivot) swap(arr[m], arr[l++]);
    
    // the position l is special, dividing <pivot and >pivot
    swap(arr[l], arr[right]);
    return l;
}

void quickSort(vector<int>& arr, int left, int right){
    if (left < right) {
        int mid = partition(arr, left, right);
        quickSort(arr, left, mid - 1);
        quickSort(arr, mid + 1, right);
    }
}

int main() {
    vector<int> arr;
    ifstream inflow(IN_FILENAME);
    int temp;
    while (inflow >> temp) arr.push_back(temp);
    inflow.close();

    clock_t T = clock();
    quickSort(arr, 0, arr.size());
    double dt = double(clock() - T);

    cout << "排序耗时" << dt << "ms." << endl;
}