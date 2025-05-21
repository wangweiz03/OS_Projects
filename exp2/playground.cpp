#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <ctime>
#include "mingw.thread.h"
#include "mingw.mutex.h"
using namespace std;

#define IN_FILENAME "data.txt" // 随机数文件名
#define OT_FILENAME "processed.txt" // 排序后文件名
#define THRES 1000 // 下限
#define THPOOL 20  // 线程池

struct message { // 描述了任务信息： 
    int left;         // 1) 左头 
    int right;        // 2) 右头
};
typedef struct message message;

class messageQueue {
private:
    mutex mtx_queue; // 互斥访问
    queue<message> messages; // 队列长度messages.size()为同步量，运行quickSort分支时++，线程池接单--
public:
    void send(int l, int r) {
        mtx_queue.lock();
        messages.push({l, r});
        // cout << "需要新线程处理" << l << "," << r << "段" << endl;
        mtx_queue.unlock();
    }
    message receive() {
        mtx_queue.lock();
        message msg = messages.front();
        messages.pop();
        // cout << "新线程开始处理" << msg.left << "," << msg.right << "段" << endl;
        mtx_queue.unlock();
        return msg;
    }
    bool isEmpty() {
        return messages.empty();
    }
};
messageQueue m_queue;

vector<int> arr; // 待排序数组

int partition(vector<int>& arr, int left, int right){ // quickSort算法核心模块
    int pivot = arr[right];
    int l = left;
    for (int m = left; m < right; ++m)
        if (arr[m] < pivot) swap(arr[m], arr[l++]);
    
    // the position l is special, dividing <pivot and >pivot
    swap(arr[l], arr[right]);
    return l;
}

void quickSort_withThread(vector<int>& arr, int left, int right){ // 融合线程调用操作的quickSort; 【任务生产者】
    if (left < right) {
        int mid = partition(arr, left, right);
        if (right - left < THRES) { // 数组长度低于阈值，无需交付新线程，自行串行解决
            quickSort_withThread(arr, left, mid - 1);
            quickSort_withThread(arr, mid + 1, right);
        } else { // 数组长度高于阈值，留待线程处理，加入队列，相当于顾客到来
            if (left < mid-1 && mid+1 < right) { // 存在双分支，于消息队列保存其一供新线程用，原线程继续执行另一半，节约线程
                m_queue.send(left, mid-1);
                quickSort_withThread(arr, mid+1, right); 
            }
            else if (left < mid-1) { // 事实上只有单分支，原线程继续执行
                quickSort_withThread(arr, left, mid-1);
            }
            else if (mid+1 < right) { // 事实上只有单分支，原线程继续执行
                quickSort_withThread(arr, mid+1, right);
            }
        }
    }
}

void Thread() { // 【任务消费者】
    while (true) {
        if (m_queue.isEmpty()) break; // 队列已空
        message msg = m_queue.receive();
        quickSort_withThread(arr, msg.left, msg.right);
    }
}

void print_arr() {
    for (int i=0; i<arr.size(); ++i) {
        cout << arr[i] << endl;
    }
    cout << arr.size() << endl;
}

int main() {
    ifstream inflow(IN_FILENAME);
    int temp;
    while (inflow >> temp) arr.push_back(temp);
    inflow.close();

    // clock_t T = clock();
    // quickSort(arr, 0, arr.size());
    // double dt = double(clock() - T);

    vector<thread> threads(THPOOL);
    clock_t T = clock();
    quickSort_withThread(arr, 0, arr.size());
    for (int i=0; i<threads.size(); ++i) threads[i] = thread(Thread);
    for (int i=0; i<threads.size(); ++i) threads[i].join();
    double dt = double(clock() - T);

    // print_arr();
    cout << "排序耗时" << dt << "ms." << endl;

    ofstream outflow(OT_FILENAME);
    for (int d : arr) outflow << d << endl;
    outflow.close();
}