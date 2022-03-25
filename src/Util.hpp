#ifndef UTIL_HPP
#define UTIL_HPP
#include <queue>
#include <mutex>
#include <condition_variable>

namespace dkml
{
    template<class T>
    class blocking_queue{
    public:
        T pop()
        {
            bool queue_empty = false;
            {
                std::lock_guard<std::mutex>lock(rw_lock);
                queue_empty = queue.empty();
            }
            if (queue_empty)
            {    
                std::unique_lock<std::mutex> lock(block_on_empty_m);
                block_on_empty.wait(lock);
            }    
            std::lock_guard<std::mutex>lock(rw_lock);
            T value = queue.front();
            queue.pop();
            return value;
        }
        int push(T value)
        {
            bool queue_empty;
            {
                std::lock_guard<std::mutex>lock(rw_lock);
                queue_empty = queue.empty();
                queue.push(value);
            }
            if (queue_empty)
            {
                block_on_empty.notify_all();
            }
            return 0;
        } 
        
    private:
       std::queue<T> queue;
       std::mutex rw_lock;
       std::condition_variable block_on_empty;
       std::mutex block_on_empty_m;

    }; 
} // namespace dkml


#endif