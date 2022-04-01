#ifndef RINGBUFFER_QUEUE_HPP
#define RINGBUFFER_QUEUE_HPP

#include <array>
#include <mutex>
#include <cmath>
#include <optional>
#include <atomic>

// The atomic queue was quicker in testing for SPSC usage, however it is important
// to keep in mind that the atomic queue will not work for MP or MC workloads
// and therefore the blocking queue is the preffered solution for MPMC queues
// here
namespace WnLSL
{
    // N here is the exponent of the power of two
    // ie blocking_rb_queue<int, 4> will give you a ringbuffer that can
    // accomodate 16 int values. 2^4 = 16
    //
    // All operations on this queue is blocking
    template <class T, int Exponent = 4>
    class blocking_rb_queue
    {
        unsigned int bitmask = (1u << Exponent) - 1;
        std::array<T, static_cast<long unsigned int>(pow(2, Exponent))> buffer{};
        unsigned int read{0};
        unsigned int write{0};
        std::atomic_flag isFull;
        std::atomic_flag isEmpty;
        // This is a pretty efficient spinlock implementation but in testing it
        // does look like the mutex is winning out, from some research modern
        // mutexes infact behave as a spinlock in situations where there is a
        // very short critical section
        //
        // WnLSL::spinlock rw_lock;
        std::mutex rw_lock;
        bool is_empty() const
        {
            return write == read;
        }

        bool is_full() const
        {
            return ((write + 1) & bitmask) == (read & bitmask);
        }

    public:
        bool enqueue(const T &item)
        // This will block until an enqueue succeeds, it always return true
        // but we kept the signature the same
        {
            for (;;)
            {
                isFull.wait(true); // Not strictly needed but prevents a busy loop
                std::scoped_lock lock(rw_lock);
                if (isFull.test())
                    continue;
                buffer[write++ & bitmask] = item;
                if (isEmpty.test())
                {
                    isEmpty.clear();
                    isEmpty.notify_all();
                }
                if (this->is_full())
                    isFull.test_and_set();
                return true;
            }
        }

        std::optional<T> dequeue()
        // This will block until a dequeue succeeds, it will always return a T
        // but we kept the signature the same
        {
            for (;;)
            {
                isEmpty.wait(true); // Not strictly needed but prevents a busy loop
                std::scoped_lock lock(rw_lock);
                if (isEmpty.test())
                    continue;
                if (isFull.test())
                {
                    isFull.clear();
                    isFull.notify_all();
                }
                if (this->is_empty())
                {
                    isEmpty.test_and_set();
                    continue;
                }
                return buffer[(read++) & bitmask];
            }
        }

        bool enqueue_noblock(const T &item)
        // This will return false if the queue is full
        {
            std::scoped_lock lock(rw_lock);
            if (this->is_full())
                return false;
            buffer[write++ & bitmask] = item;
            return true;
        }

        std::optional<T> dequeue_noblock()
        // This will return an empty optional(false for all intents and purposes)
        // if the queue is empty
        {
            std::scoped_lock lock(rw_lock);
            if (this->is_empty())
                return {};
            return buffer[(read++) & bitmask];
        }
    };

    template <class T, unsigned int EXPONENT = 4>
    class atomic_rb_queue
    {
        uint64_t bitmask = (1u << EXPONENT) - 1;
        std::array<T, static_cast<long unsigned int>(pow(2, EXPONENT))> data_{};
        alignas(64) std::atomic<unsigned int> readIdx_{0};
        alignas(64) unsigned int writeIdxCached_{0};
        alignas(64) std::atomic<unsigned int> writeIdx_{0};
        alignas(64) unsigned int readIdxCached_{0};

    public:
        bool enqueue(const T &val)
        {
            auto const writeIdx = writeIdx_.load(std::memory_order::relaxed);
            auto nextWriteIdx = writeIdx + 1;
            if (nextWriteIdx == readIdxCached_)
            {
                readIdxCached_ = readIdx_.load(std::memory_order::acquire);
                if (nextWriteIdx == readIdxCached_)
                {
                    return false;
                }
            }
            data_[writeIdx & bitmask] = val;
            writeIdx_.store(nextWriteIdx, std::memory_order::release);
            return true;
        }

        std::optional<T> dequeue()
        {
            auto const readIdx = readIdx_.load(std::memory_order::relaxed);
            if (readIdx == writeIdxCached_)
            {
                writeIdxCached_ = writeIdx_.load(std::memory_order::acquire);
                if (readIdx == writeIdxCached_)
                {
                    return {};
                }
            }
            T val = data_[readIdx & bitmask];
            auto nextReadIdx = readIdx + 1;
            readIdx_.store(nextReadIdx, std::memory_order::release);
            return val;
        }
    };
}
#endif