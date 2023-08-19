#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "PriorityQueue.h"

TEST_CASE("Check given examples") {
    //setup
    PriorityQueue<int> pq(1000, 2);

    SUBCASE("EXAMPLE 1") {
        std::vector<int> input = {4, 1, 3, 2, 1, 2};
        for (int x : input) {
            pq.push(x, x);
        }
        CHECK(pq.pop() == 1);
        CHECK(pq.pop() == 1);
        pq.push(1, 1);
        CHECK(pq.pop() == 2);
        CHECK(pq.pop() == 1);
        CHECK(pq.pop() == 2);
        CHECK(pq.pop() == 3);
    }

    SUBCASE("EXAMPLE 2") {
        std::vector<int> input = {4, 1, 3, 2, 2};
        for (int x : input) {
            pq.push(x, x);
        }
        CHECK(pq.pop() == 1);
        CHECK(pq.pop() == 2);
        pq.push(1, 1);
        CHECK(pq.pop() == 1);
        CHECK(pq.pop() == 2);
        CHECK(pq.pop() == 3);
        CHECK(pq.pop() == 4);
    }

    SUBCASE("EXAMPLE 3") {
        std::vector<int> input = {
                4, 1, 3, 2, 1, 4, 2, 3, 2, 4, 1, 3, 3, 5, 2, 1, 3,
                6, 1, 2, 4, 2, 4, 1, 3, 2, 1, 5, 2, 1, 1, 2, 3, 1, 1
        };
        for (int x : input) {
            pq.push(x, x);
        }

        std::vector<int> expected = {
                1, 1, 2, 1, 1, 2, 3, 1, 1, 2, 1, 1, 2, 3, 4, 1, 1,
                2, 1, 2, 3, 2, 2, 3, 4, 5, 2, 3, 3, 4, 3, 4, 5, 6, 4
        };

        std::vector<int> output;

        while (!pq.empty())
            output.push_back(pq.pop());

        CHECK(expected == output);
    }
}

TEST_CASE("Check throttle rate = 1") {
    //setup
    PriorityQueue<int> pq(1000, 1);

    std::vector<int> input = {
            1, 1, 1, 1, 2, 2, 2, 3, 3, 3
    };
    for (int x : input) {
        pq.push(x, x);
    }

    std::vector<int> expected = {
            1, 2, 3, 1, 2, 3, 1, 2, 3, 1
    };

    std::vector<int> output;

    while (!pq.empty())
        output.push_back(pq.pop());

    CHECK(expected == output);
}

TEST_CASE("Check insert item with priority = 0")
{
    //setup
    PriorityQueue<int> pq(1000, 1);

    CHECK_THROWS_AS(pq.push(0, 0), std::invalid_argument);
}


TEST_CASE("Test aging 1")
{
    //setup
    PriorityQueue<int> pq(1000, 2);

    std::vector<int> input = {1, 1, 1, 3, 3, 3, 4, 4, 4};

    for (int x : input) {
        pq.push(x, x);
    }
    pq.aging();
    // after 1 aging the input first priority queue remain 1, 1, 1
    CHECK(pq.queues_by_priority_.begin()->second.size() == 3);

    pq.aging();
    // after 2 aging the input first priority queue will be 1, 1, 1, 3, 3, 3
    CHECK(pq.queues_by_priority_.begin()->second.size() == 6);

    pq.aging();
    // after 3 aging the input first priority queue will be 1, 1, 1, 3, 3, 3, 4, 4, 4
    CHECK(pq.queues_by_priority_.begin()->second.size() == 9);

    // there will be only one queue
    CHECK(pq.queues_by_priority_.size() == 1);
}

TEST_CASE("Test aging 2")
{
    PriorityQueue<int> pq(1000, 2);
    pq.push(10, 10);
    for (int j = 0; j < 8; ++j) {
        for (int i = 0; i < 50; ++i) {
            pq.push(1, 1);
            pq.pop();
        }
    }
    // item 10 becomes priority 2
    for (int i = 0; i < 50; ++i) {
        pq.push(3, 3);
    }
    CHECK(pq.pop() == 10);
}

TEST_CASE("Test Next Priority")
{
    PriorityQueue<int> pq(1000, 2, 50, PriorityQueue<int>::PreemptiveMode::NEXT_PRIORITY);
    // push (10, 10, 11, 11, ... 59, 59)
    for (int i = 10; i < 60; ++i) {
        pq.push(i, i);
        pq.push(i, i);
    }

    // NEXT_PRIORITY preemptive mode will be activated after 50 popped
    for (int i = 0; i < 50; ++i) {
        pq.push(1, 1);
        pq.pop();
    }
    // the popped series will be 10, 11, 12, ... 59
    for (int i = 10; i < 60; ++i) {
        CHECK(pq.pop() == i);
    }
    CHECK(pq.size() == 50);
    // NEXT_PRIORITY preemptive mode deactivated after 50 popped
    pq.push(1, 1);
    pq.push(1, 1);
    pq.push(1, 1);
    CHECK(pq.pop() == 1);
    CHECK(pq.pop() == 1);
    CHECK(pq.pop() == 1);
}
