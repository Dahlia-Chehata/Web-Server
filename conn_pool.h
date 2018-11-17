#ifndef YAWS_CONNECTION_POOL_H_
#define YAWS_CONNECTION_POOL_H_

#include <vector>
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <thread>

class conn_pool
{
private:
    // need to keep track of workers so we can join them.
    std::vector<std::thread> workers;

    // the requests a conn_workers will work on.
    std::queue< std::function<void()> > requests;

    // synchronization tools
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool accept_requests;
public:
    conn_pool(size_t);
    ~conn_pool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;
};


inline conn_pool::conn_pool(size_t threads) : accept_requests(true)
{
    for(size_t i = 0;i<threads;++i)
        workers.emplace_back(
            [this]
            {
                for(;;)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{ return !this->accept_requests || !this->requests.empty(); });
                        if(!this->accept_requests && this->requests.empty())
                            return;
                        task = std::move(this->requests.front());
                        this->requests.pop();
                    }

                    task();
                }
            }
        );
}

inline conn_pool::~conn_pool()
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    accept_requests = false;
    lock.unlock();

    condition.notify_all();

    for(std::thread &worker: workers)
        worker.join();
}

template<class F, class... Args>
auto conn_pool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if(!accept_requests)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        requests.emplace([task](){ (*task)(); });
    }

    condition.notify_one();
    return res;
}


#endif // YAWS_CONNECTION_POOL_H_
