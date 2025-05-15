#include <iostream>
#include <queue>    //顾客队列
#include <vector>   //数组
#include <fstream>  //读文件
#include <ctime>    //计时
// thread, mutex heads ref: https://blog.csdn.net/Flag_ing/article/details/126967720
#include "mingw.thread.h"
#include "mingw.mutex.h"
using namespace std;

#define ALL_SERVERS 4
#define TIME_UNIT 1000
int ALL_CUSTOMERS;
typedef int semaphore;
typedef mutex mutex;

struct customer{
    public:
        int id;         // ifstream
        int t_entry;    // ifstream
        int t_need;     // ifstream
        int t_begin;    // set by server
        int t_leave;    // set by server
        int server;     // set by server

        void output(){
            cout << "顾客" << id << "\t进入时间" << t_entry << "\t服务时间" << t_need << "\t开始时间" << t_begin << "\t离开时间" << t_leave << "\t服务员" << server << endl;
        }

};

// 队列数据结构
queue<customer*> cqueue;

// 互斥
mutex mtx_cqueue; // 顾客队列结构互斥访问
// 同步，对应队列数据结构长度
semaphore cavail = 0; // 初始没有顾客（从0时刻开放到达）

// 时间
clock_t T_beg;
// 统计服务人数，用于终止线程
int served = 0;

void Server(int server_id) { // 柜员线程任务分配：等待顾客、叫号、服务顾客、结束顾客
    while (true){ // 柜员常驻
        if (served == ALL_CUSTOMERS) break; // 所有顾客服务完毕，退出
        mtx_cqueue.lock(); // P(mtx)
        if (cavail <= 0){ // 当<=0时，信号量无信号，表示没有顾客可用，柜员处于等待状态
            mtx_cqueue.unlock(); // V(mtx)
            continue;
        } else { // >0时进入服务状态
            customer* to_serve = cqueue.front();
            int duration = double(clock() - T_beg);
            to_serve->t_begin = duration / TIME_UNIT; // 记录时间
            cout << "柜员" << server_id << "叫号顾客" << to_serve->id << endl;
            cqueue.pop();
            -- cavail;
            mtx_cqueue.unlock(); // V(mtx), 涉及改变cqueue的动作到此为止，即可解锁
            
            Sleep(TIME_UNIT*to_serve->t_need); // 服务时间
            ++ served; // 统计服务完成数
            cout << "柜员" << server_id << "已服务顾客" << to_serve->id << endl;
            to_serve->t_leave = to_serve->t_begin + to_serve->t_need;
            to_serve->server = server_id;
        }
    }
}

void Customer(customer* customer) { // 顾客线程任务分配：入场取号、入列等待
    // 顾客入场并取号
    Sleep(TIME_UNIT*customer->t_entry);
    cout << "顾客(取号" << customer->id << ")到达" << endl;

    // 顾客入列等待
    mtx_cqueue.lock();
    cqueue.push(customer);
    ++ cavail; // 顾客队列+1，对柜员而言可用顾客+1
    mtx_cqueue.unlock();
}

int count_lines(ifstream& file) {
    int cnt = 0;
    string line;
    while (getline(file, line)) {
        ++ cnt;
    }
    return cnt;
}


int main(){
    ifstream dataflow("src.txt");
    ALL_CUSTOMERS = count_lines(dataflow);
    dataflow.clear();
    dataflow.seekg(0, ios::beg); // 恢复指针到文件开头

    vector<thread> thread_servers(ALL_SERVERS);
    for (int i = 0; i < ALL_SERVERS; ++i) {
        thread_servers[i] = thread(Server, i+1); // 创建柜员线程
    }

    vector<thread> thread_customers(ALL_CUSTOMERS);
    vector<customer> customers(ALL_CUSTOMERS);
    T_beg = clock(); // 计时开始
    for (int i = 0; i < ALL_CUSTOMERS; ++i) {
        dataflow >> customers[i].id >> customers[i].t_entry >> customers[i].t_need;
        // cout << customers[i].id << customers[i].t_entry << customers[i].t_need << endl;
        thread_customers[i] = thread(Customer, &customers[i]); // 创建顾客线程
    }
    dataflow.close();
    
    /* 在主线程环境下调用join()函数，
    主线程要等待所有线程工作做完，
    否则主线程将一直处于block状态 */
    for (int i = 0; i < ALL_SERVERS; ++i) { // 收拾残局
        thread_servers[i].join();
    }
    for (int i = 0; i < ALL_CUSTOMERS; ++i) { // 收拾残局
        thread_customers[i].join();
    }
    
    // 输出
    for (customer c : customers){
        c.output();
    }
    return 0;
}
