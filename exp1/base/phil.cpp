// philosopher problem
# define N 5 // number of philosophers
# define LEFT (i - 1) % N
# define RIGHT (i + 1) % N
# define THINKING 0 // philosopher is thinking
# define HUNGRY 1 // philosopher is hungry
# define EATING 2 // philosopher is eating
typedef int semaphore;
semaphore mutex = 1; // mutex = 1 means mutex is free
semaphore s[N]; // s[i] = 1 means philosopher i is free
int state[N]; // state[i] = 0 means philosopher i is thinking

void think(){
    // simulate thinking
}

void eat(){
    // EAT_DATA_BASE();
}

void philosopher(int i){
    while (true){
        think();
        take_forks(i);
        eat();
        put_forks(i);
    }
}

void take_forks(int i){
    -- mutex; // P(mutex); get the only key
    state[i] = HUNGRY; // philosopher i is hungry
    test(i); // test if philosopher i can eat
    ++ mutex; // V(mutex); release the key
    -- s[i]; // P(s[i]); wait for philosopher i to eat
}

void put_forks(int i){
    -- mutex; // P(mutex); get the only key
    state[i] = THINKING; // philosopher i is thinking
    test(LEFT); // 之所以要test(LEFT)和test(RIGHT)，是因为哲学家i放下叉子后，可能会影响到左边和右边的哲学家
    test(RIGHT); 
    ++ mutex; // V(mutex); release the key
}

void test(int i){
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING){
        state[i] = EATING; // philosopher i is eating
        ++ s[i]; // V(s[i]); philosopher i can eat
    }
}