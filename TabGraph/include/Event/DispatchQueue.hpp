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
    DispatchQueue(size_t threadsCount = 1)
        : _threads(threadsCount)
    {
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
    void Dispatch(std::function<void(void)> fun)
    {
        std::unique_lock<std::mutex> lock(_lock);
        _queue.push(fun);
        lock.unlock();
        _cv.notify_one();
    }

private:
    void _DispatchThreadHandler(void)
    {
        std::unique_lock<std::mutex> lock(_lock);
        while (_running) {
            _cv.wait(lock, [this] {
                return _queue.size() || !_running;
            });
            if (_queue.empty())
                continue;
            auto fun { std::move(_queue.front()) };
            _queue.pop();
            lock.unlock();
            fun();
            lock.lock();
        }
    }
    bool _running { true };
    std::vector<std::thread> _threads;
    std::mutex _lock;
    std::condition_variable _cv;
    std::queue<std::function<void(void)>> _queue;
};