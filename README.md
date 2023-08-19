# Thread-Safe Preemptive Priority Queue in C++

A C++ implementation of a custom priority queue designed for multi-threaded environments.


## Introduction

This repository presents a custom implementation of a priority queue with specialized behaviors:

- **Generic Type** (`T`): The priority queue is type-agnostic.
- **Priority-based**: Each item comes with a priority, "1" being the highest.
- **Throttle Rate**: A built-in mechanism to ensure a mix of priorities during deque operations.
- **Thread Safety**: Designed specifically for multi-threaded environments.

## Features

- **Fixed Size**: The queue has a pre-determined size.
- **Producer-Consumer Constraints**:
  - If the queue is full, any producer thread trying to add to it is blocked until space becomes available.
  - If the queue is empty, any consumer thread trying to retrieve an item is blocked until an item becomes available.
- **Preemptive Modes**: Includes mechanisms to prevent lower-priority items from getting stuck indefinitely:
  - **Aging**: Increases the priority of all items.
  - **Next Priority Activation**: Temporarily changes the throttle rate.

## How to Use

1. Include the `PriorityQueue.h` header in your project.
2. Create an instance of the priority queue specifying the desired max size, throttle rate, preemptive size, and preemptive mode.
3. Use the `push` and `pop` methods to add and retrieve items, respectively.

## Sample Usage

```cpp
#include "PriorityQueue.h"

int main() {
    PriorityQueue<int> pq(100);  // create a priority queue with a max size of 100

    pq.push(5, 1);  // push item with value 5 and priority 1
    int val = pq.pop();  // retrieves the item with the highest priority
}
```

## Debugging

To inspect the current state of the priority queue, you can use the `_display()` method. This is particularly helpful during development to understand the internal dynamics.

```cpp
pq._display();
```
