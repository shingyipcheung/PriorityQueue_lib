#include <iostream>
#define _DEBUG 1
#include "PriorityQueue.h"
#include <vector>
using namespace std;


int main() {
    PriorityQueue<int> pq(1000, 2);

//    std::vector<int> input = {4, 1, 3, 2, 1, 4, 2, 3, 2, 4, 1, 3, 3, 5, 2, 1, 3,
//                              6, 1, 2, 4, 2, 4, 1, 3, 2, 1, 5, 2, 1, 1, 2, 3, 1, 1};
//    for (int x : input) {
//        pq.push(x, x);
//    }
//
//    while (!pq.empty())
//        pq.pop();
    std::vector<int> input = {4, 1, 3, 2, 1};
    for (int x : input) {
        pq.push(x, x);
    }
    pq._display();
    pq.pop();
    pq.pop();
    pq.pop();
    pq.push(2, 2);
    pq._display();
    pq.push(3, 3);
    pq._display();
    pq.pop();
    pq.pop();
    pq._display();
    pq.push(1, 1);
    pq.pop();
    pq.pop();
    pq._display();
    pq.push(5, 5);
    pq.pop();
    pq.push(4, 4);
    pq._display();
    pq.pop();
    pq.push(1, 1);
    pq.pop();
    return 0;
}
