#ifndef PRIORITY_QUEUE_PRIORITYQUEUE_H
#define PRIORITY_QUEUE_PRIORITYQUEUE_H

#include <map>
#include <list>
#include <condition_variable>

template<typename T>
class PriorityQueue {
public:
    enum class PreemptiveMode {
        AGING,
        NEXT_PRIORITY,
    };
    /**
     * Constructor of PriorityQueue
     *
     * @param max_size
     * the maximum size of the priority queue
     * @param throttle_rate
     * for every {throttle_rate} items dequeued with priority "x", the next item must be of priority "x+1"
     * @param preemptive_size
     * for every {preemptive_size} items being popped, it trigger a mechanism to
                              relax the throttle_rate constraint that prevents the lower priority items never exit the queue
     * @param preemptive_mode
     * the mechanism prevents the lower priority items never exit the queue
     */
    PriorityQueue(size_t max_size, int throttle_rate=2, int preemptive_size=50, PreemptiveMode preemptive_mode=PreemptiveMode::AGING) :
            max_size_(max_size),
            size_(0),
            throttle_rate_(throttle_rate),
            PREEMPTIVE_SIZE(preemptive_size),
            preemptive_mode_(preemptive_mode),
            global_counter_(0),
            next_priority_counter_(0),
            next_priority_activated_(false)
    {
        preemptive_it_ = queues_by_priority_.end();
    }

    // push an item with a given priority
    void push(T item, int priority) {
        // Priority is a positive integer. The highest priority is “1”.
        if (priority < 1) {
            throw std::invalid_argument("Priority must be greater than or equal to 1");
        }
        std::unique_lock <std::mutex> lock(mtx_);
        // wait for buffer to be emptied
        while (size_ == max_size_) {
            empty_cv_.wait(lock);
        }
        queues_by_priority_[priority].push_back(item);
        size_++;
#ifdef _DEBUG
        std::cout << "push: " << priority << std::endl;
#endif
        lock.unlock();
        // signal that the buffer has been filled
        full_cv_.notify_one();
    }

    // pop an item
    T pop() {
        std::unique_lock <std::mutex> lock(mtx_);
        // wait for the buffer to be filled
        while (size_ == 0) {
            full_cv_.wait(lock);
        }

        auto it = queues_by_priority_.begin();
        if (preemptive_it_ != queues_by_priority_.end())
        {
            it = preemptive_it_;
        }

        T item = it->second.front();
        int priority = it->first;
        if (priority_popped_counter_.find(priority) == priority_popped_counter_.end()) {
            priority_popped_counter_[priority] = 0;
        }
        priority_popped_counter_[priority]++;

#ifdef _DEBUG
        std::cout << "pop: " << item << std::endl;
#endif
        it->second.pop_front();

        preemptive_it_ = queues_by_priority_.end();

        int rate = throttle_rate_;
        if (next_priority_activated_) {
            next_priority_counter_--;
            rate = 1;
        }
        // bonus can assign different values of throttle_rate_ based on the priority level
        if (throttle(priority, rate)) {
            // move to the next queue
            auto next = it;
            next++;
            if (next != queues_by_priority_.end())
            {
                if  (priority + 1 == next->first // x", the next item must be of priority "x+1".
                     || next_priority_activated_)
                {
                    preemptive_it_ = next; // set next preemptive
                }
            }
        }
        if (it->second.empty()) {
            queues_by_priority_.erase(it);
        }
        size_--;

        global_counter_++;

        // increase priority for all items
        if (PREEMPTIVE_SIZE != 0 && global_counter_ % PREEMPTIVE_SIZE == 0) {
            switch (preemptive_mode_) {
                case PreemptiveMode::AGING:
                    aging();
                    break;
                case PreemptiveMode::NEXT_PRIORITY:
                    _activate_next_priority_mode();
                    break;
            }
            global_counter_ = 0;
        }

        lock.unlock();
        // signal that the buffer is consumed
        empty_cv_.notify_one();
        return item;
    }

    bool empty() const {
        std::unique_lock<std::mutex> lock(mtx_);
        return size_ == 0;
    }

    size_t size() const {
        std::unique_lock<std::mutex> lock(mtx_);
        return size_;
    }

    void _display() const {
        std::unique_lock<std::mutex> lock(mtx_);
        std::cout << "----------------------------------" << std::endl;
        printf("PriorityQueue(max_size=%d, throttle_rate=%d)\n", max_size_, throttle_rate_);
        for (auto& pair: queues_by_priority_)
        {
            int priority = pair.first;
            int count = priority_popped_counter_.find(priority)->second;
            std::cout << "priority=" << priority << " popped_count=" << count << " items : ";
            for (auto& item: pair.second) {
                std::cout << item << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "----------------------------------" << std::endl;
    }

//private: comment for debugging
    /* lower priority items may never exit the queue if
     * we keep push higher priority items into the queue,
     * the aging mechanism can be used that increase the priority of each queue
     * and merge them if the queues with the same priority
    */
    void aging() {
        // invalidate preemptive_it_
        preemptive_it_ = queues_by_priority_.end();

        for (auto& pair: queues_by_priority_)
        {
            int priority = pair.first;
            if (priority != 1) {
                auto& q1 = queues_by_priority_[priority - 1];
                auto& q2 = pair.second;
                q1.splice(q1.end(), q2);
            }
        }
        // https://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
        for (auto it = queues_by_priority_.cbegin(); it != queues_by_priority_.cend(); /* no increment */)
        {
            if (it->second.empty())
                queues_by_priority_.erase(it++);    // or "it = m.erase(it)" since C++11
            else
                ++it;
        }
    }

    void _activate_next_priority_mode() {
        if (next_priority_activated_) {
            next_priority_activated_ = false;
        }
        else {
            next_priority_activated_ = true;
            next_priority_counter_ = PREEMPTIVE_SIZE;
        }
    }

    bool throttle(int priority, int rate) {
        int count = priority_popped_counter_[priority];
        if (count != 0 && count % rate == 0) {
            // reset
            priority_popped_counter_[priority] = 0;
            return true;
        }
        return false;
    }

    std::map<int, int> priority_popped_counter_;
    int global_counter_;

    const int PREEMPTIVE_SIZE;
    PreemptiveMode preemptive_mode_;
    int next_priority_counter_;
    bool next_priority_activated_;

    std::map<int, std::list<T>> queues_by_priority_;
    typename std::map<int, std::list<T>>::iterator preemptive_it_;
    size_t max_size_;
    size_t size_;
    int throttle_rate_;

    std::condition_variable empty_cv_;
    std::condition_variable full_cv_;
    mutable std::mutex mtx_;
};


#endif //PRIORITY_QUEUE_PRIORITYQUEUE_H
