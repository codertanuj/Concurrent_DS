#include <iostream>
#include <thread>
#include <vector>
#include "stack.h"  // your stack implementation

LockFreeStack stack;

void worker(int id) {
    for (int i = 0; i < 5; i++) {
        stack.push(id * 10 + i);
    }
    for (int i = 0; i < 5; i++) {
        int val=stack.pop();
        if (val!=-1) {
            std::cout << "Thread " << id << " popped " << val << "\n";
        }
    }
}

int main() {
    std::vector<std::thread> threads;

    // 🔹 Launch 4 threads
    for (int t = 0; t < 4; t++) {
        threads.emplace_back(worker, t);
    }

    // 🔹 Wait for them to finish
    for (auto &th : threads) th.join();

    return 0;
}
