// reader - writer problem
typedef int semaphore;
semaphore mutex = 1; // mutex = 1 means mutex is free
semaphore db = 1; // db = 1 means database is free, dealing with writer
int rc = 0; // number of readers

// mutex的作用是保护rc的读写，因为rc是共享变量
// READ_DATA_BASE()和WRITE_DATA_BASE()不用mutex保护，因为它是原子操作

void reader(void){ // 1个进程单元，可能同时多个程序在跑reader()
    while (true){
        -- mutex; // P(mutex); get the only key
        ++ rc; // one more reader
        if (rc == 1){
            -- db; // P(db);
        }
        ++ mutex; // V(mutex); release the key

        // READ_DATA_BASE();

        -- mutex; // P(mutex); get the only key
        -- rc; // one less reader
        if (rc == 0){
            ++ db; // V(db);
        }
        ++ mutex; // V(mutex); release the key

        // NONCRITICAL();

    }
}

void writer(void){
    while (true){

        // NONCRITICAL();

        -- db; // P(db); get the database

        // WRITE_DATA_BASE();
        
        ++ db; // V(db); release the database

    }
}