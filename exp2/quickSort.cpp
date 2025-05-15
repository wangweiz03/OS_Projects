#include <iostream>
#include <vector>
using namespace std;

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
    vector<int> arr = {1,4,3,2,5,7,6,10,8};
    quickSort(arr, 0, arr.size()-1);
    for (int a : arr) cout << a << " ";
    return 0;
}