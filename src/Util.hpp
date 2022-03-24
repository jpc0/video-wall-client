#ifndef UTIL_HPP
#define UTIL_HPP
#include <queue>
#include <mutex>

namespace dkml
{
    class locked_queue{
    public:
        int push(void *);
        void* pop();
    private:
       std::queue<void *> queue;
       std::mutex rw_lock;
    }; 
} // namespace dkml


#endif