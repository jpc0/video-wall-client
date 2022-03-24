#include "Util.hpp"

namespace dkml
{
    void * locked_queue::pop()
    {
        std::lock_guard<std::mutex>lock(rw_lock);
        if (queue.empty())
            return 0;
        void *value = queue.front();
        queue.pop();
        return value;
    }

    int locked_queue::push(void *value)
    {
        std::lock_guard<std::mutex>lock(rw_lock);
        queue.push(value);
        return 0;
    } 
} // namespace dkml
