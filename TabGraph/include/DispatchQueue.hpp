/*
* @Author: gpinchon
* @Date:   2021-05-30 22:32:44
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-30 22:33:52
*/

#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <functional>

struct DispatchQueue {
    struct TaskIdentifier {
        uint64_t dispatchQueueID;
        uint64_t taskID;
    };
    DispatchQueue(size_t threadsCount = 1)
        : _threads(threadsCount)
    {
        static uint64_t s_ID{0};
        _ID = ++s_ID;
        for (auto& thread : _threads)
            thread = std::thread(&DispatchQueue::_DispatchThreadHandler, this);
    }
    ~DispatchQueue()
    {
        _running = false;
        _cv.notify_all();
        for (auto& thread : _threads)
            if (thread.joinable())
                thread.join();
    }
    TaskIdentifier Dispatch(std::function<void(void)> fun)
    {
        std::unique_lock<std::mutex> lock(_lock);
        _queue.push({fun, ++_taskID});
        lock.unlock();
        _cv.notify_one();
        return { _ID, _taskID };
    }
    /**
     * @brief use this to dispatch an event, threadsCount is defined at compilation time
     * @return the application's DispatchQueue
    */
    static DispatchQueue& ApplicationDispatchQueue();

private:
    void _DispatchThreadHandler(void);
    uint64_t _ID{ 0 };
    uint64_t _taskID { 0 };
    bool _running { true };
    std::vector<std::thread> _threads;
    std::mutex _lock;
    std::condition_variable _cv;
    std::queue<std::pair<std::function<void(void)>, uint64_t>> _queue;
};