#pragma once

#include <thread>
#include <future>
#include <queue>

namespace asdf
{
    //todo: move this somewhere better
    template <class T>
    struct array_view_
    {
        T* data;
        size_t count;
    };


    using task_t = std::thread;

    struct thread_pool_t
    {
        std::queue<task_t> queued_tasks;

        std::vector<task_t> active_task;

        void add_task();
    };

    template <typename L, typename F>
    //where L is an array/list of some kind {T* data, size_t count}; F is a function
    struct divided_task_
    {
        L output_buffer;

        std::vector<std::future<void>> task_futures;

        divided_task_(L _buffer, F&& task_func, size_t num_sub_tasks = 5)
        : output_buffer(_buffer)
        {
            ASSERT(output_buffer.count % num_sub_tasks == 0, "todo: deal with the fact that num_sum_tasks may not divide evenly into the buffer size");
            size_t num_elements = output_buffer.count / num_sub_tasks;

            for(size_t task_index = 0; task_index < num_sub_tasks; ++task_index)
            {
                auto future = std::async(
                    [_task_func = std::move(task_func)](size_t start_index, size_t end_index)
                    {
                        for(size_t i = start_index; i < end_index; ++i)
                        {
                          _task_func(i);
                        }
                    },
                    num_elements * task_index, num_elements * task_index + num_elements
                );

                task_futures.push_back(std::move(future));
            }
        }
    };
}