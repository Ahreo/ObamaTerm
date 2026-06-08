#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <chrono>
#include <mutex>
#include <deque>

template <typename T>
class BlockingQueue {
public: 
    void push(T item);
    bool tryPop(T& out);
    bool waitPopFor(T& out, std::chrono::milliseconds timeout);

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<T> queue_;
};

#endif // BLOCKING_QUEUE_H
