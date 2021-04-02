#pragma once
#include <thread>
#include <optional>

template<typename T>
class Task
{
    public:
        template<typename ... Args>
        Task(Args&& ... args):
            m_runnable{args...}
        {}

        ~Task()
        {
            if(m_thread && m_thread->joinable())
            {
                m_thread->join();
            }
        }

        Task(const Task&) = delete; // no copy
        void operator=(const Task&) = delete; // no assignment

        // move??

        template<typename ... Args>
        void start(Args&& ... args)
        {
            m_thread = std::thread{[this, args = std::make_tuple(std::forward<Args>(args) ...)](){
                std::apply([this](auto&& ... args)
                {
                    m_runnable.run(args...);
                }, std::move(args));
            }};
        }

        template<typename ... Args>
        void stop(Args&& ... args)
        {
            m_runnable.stop(args...);
        }

    private:

    T m_runnable;
    std::optional<std::thread> m_thread;
};