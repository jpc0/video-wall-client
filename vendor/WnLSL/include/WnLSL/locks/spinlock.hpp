#ifndef SPINLOCK_HPP
#define SPINLOCK_HPP

#include <atomic>

namespace WnLSL
{
    class spinlock
    {
        std::atomic_flag lock_;

    public:
        void lock() noexcept
        {
            while (lock_.test_and_set(std::memory_order::acquire))
            {
#if defined(__cpp_lib_atomic_flag_test)
                while (lock_.test(std::memory_order::relaxed))
#endif
                    __builtin_ia32_pause();
            }
        }

        bool try_lock() noexcept
        {
            return !lock_.test_and_set(std::memory_order::relaxed);
        }

        void unlock() noexcept
        {
            lock_.clear(std::memory_order::release);
        }
    };
}
#endif
